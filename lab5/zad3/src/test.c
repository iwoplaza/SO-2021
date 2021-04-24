#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <fcntl.h>
#include <limits.h>
#include "aggregate.h"

const char* USAGE = "<producers> <consumers> <pipe> <output> <batch_size>";
const char* DATA_FILENAME_TEMPLATE = "data_%d.txt";

typedef struct Args_t {
    int producers;
    int consumers;
    const char* pipe_path;
    const char* output_filepath;
    const char* batch_size;
} Args_t;

void read_and_validate_args(int argc, char** argv, Args_t* args_out)
{
    if (argc != 6)
    {
        printf("USAGE: %s\n", USAGE);
        exit(0);
    }

    args_out->producers = atoi(argv[1]);
    args_out->consumers = atoi(argv[2]);
    args_out->pipe_path = argv[3];
    args_out->output_filepath = argv[4];
    args_out->batch_size = argv[5];
}

void create_producer(const Args_t* args, int producer_id)
{
    char id_buffer[16];
    sprintf(id_buffer, "%d", producer_id);

    char input_file[16];
    sprintf(input_file, DATA_FILENAME_TEMPLATE, producer_id);

    close(STDOUT_FILENO);
    execl("producer", "producer", args->pipe_path, id_buffer, input_file, args->batch_size, NULL);
}

void create_consumer(const Args_t* args)
{
    close(STDOUT_FILENO);
    execl("consumer", "consumer", args->pipe_path, args->output_filepath, args->batch_size, NULL);
}

bool test_integrity(const Args_t* args)
{
    int fd = open(args->output_filepath, O_RDONLY);
    Aggregate_t* aggregate = read_aggregate(fd);

    for (int i = 0; i < args->producers; ++i)
    {
        char input_file[16];
        sprintf(input_file, DATA_FILENAME_TEMPLATE, i);

        int in_fd = open(input_file, O_RDONLY);
        Aggregate_t* data_aggregate = read_aggregate(in_fd);

//        print_aggregate(data_aggregate);

        if (data_aggregate->count == 0 || cmp_aggregate_line(aggregate, i, data_aggregate->lines[0].value) == false)
        {
            free_aggregate(data_aggregate);
            return false;
        }

        free_aggregate(data_aggregate);
    }

    free_aggregate(aggregate);

    return true;
}

int main(int argc, char** argv)
{
    printf("PIPE BUFFER SIZE: %d\n", PIPE_BUF);

    Args_t args;
    read_and_validate_args(argc, argv, &args);

    // Removing the old output, if there was any.
    remove(args.output_filepath);

    for (int i = 0; i < args.consumers; ++i)
    {
        if (fork() == 0)
        {
            // We are the consumer
            create_consumer(&args);
            return 0;
        }
    }

    for (int i = 0; i < args.producers; ++i)
    {
        if (fork() == 0)
        {
            // We are the producer
            create_producer(&args, i);
            return 0;
        }
    }

    // Waiting for child processes to end.
    int status = 0;
    while ((wait(&status)) > 0);

    printf("All processes finished their jobs.\n");
    printf("Testing integrity... ");

    if (test_integrity(&args))
    {
        printf("OK\n\n");
    }
    else
    {
        printf("BAD OUTPUT\n\n");
    }
}