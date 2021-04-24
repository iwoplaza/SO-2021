#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "utils.h"
#define BUFFER_SIZE 128

const char* USAGE = "<pipe> <output> <batch_size>";

typedef struct Args_t {
    int batch_size;
    const char* output_filepath;
    const char* pipe_path;
} Args_t;

void read_and_validate_args(int argc, char** argv, Args_t* args_out)
{
    if (argc != 4)
    {
        printf("USAGE: %s\n", USAGE);
        exit(0);
    }

    args_out->pipe_path = argv[1];
    args_out->output_filepath = argv[2];
    args_out->batch_size = atoi(argv[3]);

    if (args_out->batch_size <= 0)
    {
        fprintf(stderr, "[INVALID ARG]: The batch size has to be at least 1.\n");
        exit(1);
    }
}

typedef struct AggregateLine_t {
    char* value;
    int capacity;
    int count;
} AggregateLine_t;

typedef struct Aggregate_t {
    AggregateLine_t* lines;
    int capacity;
    int count;
} Aggregate_t;

void aggregate_add_line(Aggregate_t* aggregate)
{
    AggregateLine_t* line = &aggregate->lines[aggregate->count];
    line->capacity = 32;
    line->value = malloc(line->capacity);
    line->count = 0;

    aggregate->count++;

    if (aggregate->count >= aggregate->capacity)
    {
        aggregate->capacity += 16;
        aggregate->lines = realloc(aggregate->lines, sizeof(AggregateLine_t) * aggregate->capacity);
    }
}

void aggregate_add_data(Aggregate_t* aggregate, int line_idx, char* data, int data_size)
{
    // While the last index in the aggregate is smaller than the line idx.
    while (aggregate->count - 1 < line_idx)
    {
        aggregate_add_line(aggregate);
    }

    AggregateLine_t* line = &aggregate->lines[line_idx];
    
    // We account for the future '\0' symbol.
    int min_capacity = line->count + data_size + 1;
    if (line->capacity < min_capacity)
    {
        // Readjusting the capacity
        line->capacity = min_capacity;
        line->value = realloc(line->value, sizeof(char) * line->capacity);
    }

    memcpy(line->value + line->count, data, data_size);
    line->count += data_size;
}

Aggregate_t* read_aggregate(FILE* file)
{
    char buffer[BUFFER_SIZE];
    int data_size = 0;

    Aggregate_t* aggregate = malloc(sizeof(Aggregate_t));
    aggregate->capacity = 16;
    aggregate->lines = malloc(sizeof(AggregateLine_t) * aggregate->capacity);
    aggregate->count = 0;

    int line_idx = 0;

    while ((data_size = fread(buffer, sizeof(char), BUFFER_SIZE, file)) > 0)
    {
        printf("Read %d bytes from file.\n", data_size);

        int data_idx = 0;

        while (data_idx < data_size)
        {
            int line_data_size = data_size - data_idx;

            char* endline_ptr = strchr(buffer + data_idx, '\n');
            if (endline_ptr != NULL)
            {
                // We've reached an end of line.
                int line_data_size = endline_ptr - (buffer + data_idx);
                aggregate_add_data(aggregate, line_idx, buffer, line_data_size);

                line_idx++;
            }
            else
            {
                // We're continuing to read the line.
                aggregate_add_data(aggregate, line_idx, buffer, line_data_size);
            }

            data_idx += line_data_size;
        }
    }

    return aggregate;
}

void write_aggregate(FILE* file)
{

}

void free_aggregate(Aggregate_t* aggregate)
{
    for (int i = 0; i < aggregate->count; ++i)
    {
        free(aggregate->lines[i].value);
    }
    free(aggregate->lines);

    free(aggregate);
}

int main(int argc, char** argv)
{
    Args_t args;
    read_and_validate_args(argc, argv, &args);

    // Greeting message
    printf("=== CONSUMER ===\n");
    printf("Reading from '%s', '%d' elements per batch to '%s'.\n\n", args.pipe_path, args.batch_size, args.output_filepath);

    FILE* output_file = open_resource("output file", args.output_filepath, "rw");

    Aggregate_t* aggregate = read_aggregate(output_file);

    FILE* pipe = open_resource("FIFO", args.pipe_path, "r");

    // Freeing all resources.
    free_aggregate(aggregate);
    fclose(pipe);
    fclose(output_file);

    return 0;
}
