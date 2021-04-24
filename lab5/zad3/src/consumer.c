#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>
#include "utils.h"
#include "aggregate.h"

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

size_t read_pipe(FILE* pipe, int batch_size, char* data_buffer, int* line_idx)
{
    char buffer;

    *line_idx = 0;

    // Reading the line idx.
    while (fread(&buffer, sizeof(char), 1, pipe) > 0)
    {
        if (buffer == ' ')
        {
            break;
        }

        if (buffer < '0' || buffer > '9')
        {
            fprintf(stderr, "Received an invalid producer index, expected a digit, got '%c'\n", buffer);
            exit(1);
        }

        // Multiplying the previous digit by 10, adding a new digit.
        *line_idx = 10 * (*line_idx) + (buffer - '0');
    }

    return fread(data_buffer, sizeof(char), batch_size, pipe);
}

size_t get_actual_data_size(const char* buffer, size_t data_size)
{
    for (int i = 0; i < data_size; ++i)
    {
        if (buffer[i] == '\0')
        {
            return i;
        }
    }

    return data_size;
}

int main(int argc, char** argv)
{
    Args_t args;
    read_and_validate_args(argc, argv, &args);

    // Greeting message
    printf("=== CONSUMER ===\n");
    printf("Reading from '%s', '%d' elements per batch to '%s'.\n\n", args.pipe_path, args.batch_size, args.output_filepath);

    FILE* pipe = open_resource_std("FIFO", args.pipe_path, "r");

    char* buffer = malloc(sizeof(char) * args.batch_size);
    size_t data_size = 0;

    int line_idx = 0;

    while ((data_size = read_pipe(pipe, args.batch_size, buffer, &line_idx)) > 0)
    {
        if (data_size != args.batch_size)
        {
            fprintf(stderr, "Invalid data length. Expected data size to be a multiply of <batch_size>\n");
            exit(1);
        }

        // Opening the output file
        int input_file = open_resource("input file", args.output_filepath, O_CREAT|O_RDONLY, S_IRWXU);

        // Locking the file
        flock(input_file, LOCK_EX);

        // Reading the previous contents
        Aggregate_t *aggregate = read_aggregate(input_file);
        close(input_file);

        // Adding new content to the aggregate
        aggregate_add_data(aggregate, line_idx, buffer, get_actual_data_size(buffer, data_size));

        // Saving the new contents
        int output_file = open_resource("output file", args.output_filepath, O_WRONLY|O_TRUNC, 0);
        write_aggregate(aggregate, output_file);
        close(output_file);

        // Unlocking the file
        flock(input_file, LOCK_UN);

        // Freeing memory.
        free_aggregate(aggregate);
    }

    // Freeing all resources.
    fclose(pipe);
    free(buffer);

    return 0;
}
