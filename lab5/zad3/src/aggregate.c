#include "aggregate.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define BUFFER_SIZE 128

static int index_of(char find, const char* buffer, int buffer_size)
{
    for (int i = 0; i < buffer_size; ++i)
    {
        if (buffer[i] == find)
            return i;
    }

    return -1;
}

void aggregate_add_line(Aggregate_t* aggregate)
{
    AggregateLine_t* line = &aggregate->lines[aggregate->count];
    line->capacity = 32;
    line->value = malloc(line->capacity);
    // Making sure that the last value is followed by a \0.
    line->value[0] = '\0';
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

    // Making sure the line ends with '\0'.
    line->value[line->count] = '\0';
}

Aggregate_t* read_aggregate(int file)
{
    char buffer[BUFFER_SIZE];
    int data_size;

    Aggregate_t* aggregate = malloc(sizeof(Aggregate_t));
    aggregate->capacity = 16;
    aggregate->lines = malloc(sizeof(AggregateLine_t) * aggregate->capacity);
    aggregate->count = 0;

    int line_idx = 0;

    while ((data_size = read(file, buffer, BUFFER_SIZE)) > 0)
    {
        int data_idx = 0;

        while (data_idx < data_size)
        {
            int endline_idx = index_of('\n', buffer + data_idx, data_size - data_idx);
            if (endline_idx != -1)
            {
                // We've reached an end of line.
                aggregate_add_data(aggregate, line_idx, buffer + data_idx, endline_idx);

                // Passing by the newline
                data_idx += endline_idx + 1;

                line_idx++;
            }
            else
            {
                int line_data_size = data_size - data_idx;

                // We're continuing to read the line.
                aggregate_add_data(aggregate, line_idx, buffer + data_idx, line_data_size);

                data_idx += line_data_size;
            }
        }
    }

    return aggregate;
}

void write_aggregate(Aggregate_t* aggregate, int file)
{
    for (int i = 0; i < aggregate->count; ++i)
    {
        // Making sure the lines end with '\n'.
        char* print_buffer = malloc(sizeof(char) * (aggregate->lines[i].count + 1));
        memcpy(print_buffer, aggregate->lines[i].value, aggregate->lines[i].count);
        print_buffer[aggregate->lines[i].count] = '\n';

        write(file, print_buffer, aggregate->lines[i].count + 1);

        free(print_buffer);
    }
}

void print_aggregate(Aggregate_t* aggregate)
{
    for (int i = 0; i < aggregate->count; ++i)
    {
        printf("%s\n", aggregate->lines[i].value);
    }
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

bool cmp_aggregate_line(Aggregate_t* a, int line_idx, const char* value)
{
    if (line_idx >= a->count)
        return false;

    return strcmp(a->lines[line_idx].value, value) == 0;
}