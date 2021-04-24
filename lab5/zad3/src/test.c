#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

const char* USAGE = "<producers> <pipe> <output> <batch_size>";

typedef struct Args_t {
    int producers;
    int consumers;
    const char* pipe_path;
    const char* output_filepath;
    const char* batch_size;
} Args_t;

void read_and_validate_args(int argc, char** argv, Args_t* args_out)
{
    if (argc != 5)
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
    sprintf(input_file, "data_%d.txt", producer_id);

    execl("producer", "producer", args->pipe_path, id_buffer, input_file, args->batch_size, NULL);
}

void create_consumer(const Args_t* args)
{
    execl("consumer", "consumer", args->pipe_path, args->output_filepath, args->batch_size, NULL);
}

int main(int argc, char** argv)
{
    Args_t args;
    read_and_validate_args(argc, argv, &args);

    for (int i = 0; i < args.consumers; ++i)
    {
        if (fork() == 0)
        {
            // We are the consumer
            create_consumer(&args);
            return 0;
        }

        printf("Created customer %d.\n", i);
    }

    for (int i = 0; i < args.producers; ++i)
    {
        if (fork() == 0)
        {
            // We are the producer
            create_producer(&args, i);
            return 0;
        }

        printf("Created producer %d.\n", i);
    }

    // Waiting for child processes to end.
    int status = 0;
    while ((wait(&status)) > 0);

    printf("All processes finished their jobs.\n");
}