#include <stdio.h>
#include <phread.h>
#include "region.h"

#define NUMBERS_MODE_STR "numbers"
#define BLOCKS_MODE_STR "block"
#define MODES_STR NUMBERS_MODE_STR "|" BLOCKS_MODE_STR

typedef struct ThreadArgs_t {
    RegionSet_t regionset;
} ThreadArgs_t;

typedef enum SplitMode_t {
    SPLIT_NUMBERS,
    SPLIT_BLOCKS,
} SplitMode_t;

void parse_arguments();
SplitMode_t parse_mode();
void thread_routine();
int create_thread();

int thread_count;
SplitMode_t mode;
const char* input_filepath;
const char* output_filepath;

int main(int argc, char** argv)
{
    if (argc != 5)
    {
        printf("Usage: <thread_count> " MODES_STR " <input_file> <output_file>");
        return 0;
    }

    // Parsing arguments.
    parse_arguments();

    return 0;
}

void parse_arguments()
{
    thread_count = atoi(argv[1]);
    mode = parse_mode(argv[2]);
    input_filepath = argv[3];
    output_filepath = argv[4];

    // Checking validity
    if (thread_count <= 0)
    {
        fprintf(stderr, "There has to be at least 1 worker thread.\n");
        exit(1);
    }

    if (mode == -1)
    {
        fprintf(stderr, "Unsupported mode: %s. Available modes: " MODES_STR ".\n", argv[2]);
        exit(1);
    }

    FILE* input_file = fopen(input_filepath, "r");
    if (input_file == NULL)
    {
        fprintf(stderr, "Couldn't open input file: %s.\n", input_filepath);
        exit(1);
    }

    FILE* output_file = fopen(output_filepath, "w");
    if (output_file == NULL)
    {
        fprintf(stderr, "Couldn't open output file: %s.\n", output_filepath);
        exit(1);
    }
}

void thread_routine()
{

}

int create_thread()
{
    pthread_t* thread;

    ThreadArgs_t* thread_args = malloc(sizeof(ThreadArgs_t));

    if (pthread_create(&thread, NULL, thread_routine, thread_args) == -1)
    {
        perror("Failed to create thread.");
        return -1;
    }

    return 0;
}
