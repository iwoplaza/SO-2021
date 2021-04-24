#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "utils.h"

const char* USAGE = "<pipe> <id> <input> <batch_size>";

typedef struct Args_t {
    int producer_id;
    int batch_size;
    const char* input_filepath;
    const char* pipe_path;
} Args_t;

void read_and_validate_args(int argc, char** argv, Args_t* args_out)
{
    if (argc != 5)
    {
        printf("USAGE: %s\n", USAGE);
        exit(0);
    }

    args_out->pipe_path = argv[1];
    args_out->producer_id = atoi(argv[2]);
    args_out->input_filepath = argv[3];
    args_out->batch_size = atoi(argv[4]);

    bool valid = true;

    if (args_out->producer_id < 0)
    {
        fprintf(stderr, "[INVALID ARG]: The producer id has to be at least 0.\n");
        valid = false;
    }

    if (args_out->batch_size <= 0)
    {
        fprintf(stderr, "[INVALID ARG]: The batch size has to be at least 1.\n");
        valid = false;
    }

    if (!valid)
    {
        exit(1);
    }
}

void send_to_pipe(FILE* pipe, int producer_id, char* data, int batch_size)
{
    // Sending the producer id, then a space before the upcoming data.
    fprintf(pipe, "%d ", producer_id);

    // Sending the data
    // Note that the actual data might be smaller in size than the batch size,
    // but at the end of the actual data, there will be a '\0' character, so
    // the consumer will know to stop either at the batch_size or when
    // meeting that '\0' character.
    fwrite(data, sizeof(char), batch_size, pipe);
}

float randf()
{
    return (float) rand() / (float) (RAND_MAX);
}

int main(int argc, char** argv)
{
    Args_t args;
    read_and_validate_args(argc, argv, &args);

    // Greeting message
    printf("=== PRODUCER [%d] ===\n", args.producer_id);
    printf("[%d] Reading from '%s', '%d' elements per batch to '%s'.\n\n", args.producer_id, args.input_filepath, args.batch_size, args.pipe_path);

    FILE* pipe = open_resource_std("FIFO", args.pipe_path, "w");
    FILE* data_file = open_resource_std("data file", args.input_filepath, "r");

    // Initializing the data buffer.
    char* data_buffer = malloc(sizeof(char) * (args.batch_size + 1));

    size_t len_read;
    while ((len_read = fread(data_buffer, sizeof(char), args.batch_size, data_file)) > 0)
    {
        data_buffer[len_read] = '\0';

        printf("%s\n", data_buffer);
        send_to_pipe(pipe, args.producer_id, data_buffer, args.batch_size);

        // Waiting for a random amount of seconds between 1-2s.
        sleep(1 + randf());
    }

    // Freeing all resources.
    free(data_buffer);
    fclose(pipe);
    fclose(data_file);

    return 0;
}
