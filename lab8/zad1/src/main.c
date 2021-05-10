#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timeutil.h"
#include "region.h"
#include "pgm.h"

#define COL_RESET "\033[0m"
#define COL_YELLOW "\033[1;33m"
#define COL_BLUE "\033[0;34m"
#define COL_CYAN "\033[0;36m"
#define COL_ERROR "\033[0;31m"

#define NUMBERS_MODE_STR "numbers"
#define BLOCKS_MODE_STR "block"
#define MODES_STR NUMBERS_MODE_STR "|" BLOCKS_MODE_STR

typedef struct ThreadArgs_t {
    int thread_idx;
    RegionSet_t regionset;
} ThreadArgs_t;

typedef enum SplitMode_t {
    SPLIT_NUMBERS,
    SPLIT_BLOCKS,
} SplitMode_t;

static void parse_arguments();
static SplitMode_t parse_mode();
static void load_image();
static void run_jobs();
static void* thread_routine(void*);
static pthread_t create_thread();

int thread_count;
SplitMode_t mode;
const char* input_filepath;
const char* output_filepath;
PGMImage_t image;
FILE* output_file;

int main(int argc, char** argv)
{
    if (argc != 5)
    {
        printf("Usage: <thread_count> " MODES_STR " <input_file> <output_file>");
        return 0;
    }

    // Parsing arguments.
    parse_arguments(argv);

    // Parsing input image.
    load_image();

    // Running threads
    run_jobs();

    // Opening output file.
    output_file = fopen(output_filepath, "w");
    if (output_file == NULL)
    {
        fprintf(stderr, "Couldn't open output file: %s.\n", output_filepath);
        return 1;
    }

    if (pgm_write(output_file, &image) != PGM_SUCCESS)
    {
        fprintf(stderr, "Couldn't write PGM image '%s': %s\n", output_filepath, pcm_get_error());
        return 1;
    }

    fclose(output_file);

    // Cleaning
    pgm_free(&image);

    return 0;
}

void parse_arguments(char** argv)
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
}

SplitMode_t parse_mode(const char* in)
{
    if (strcmp(in, NUMBERS_MODE_STR) == 0)
        return SPLIT_NUMBERS;
    else if (strcmp(in, BLOCKS_MODE_STR) == 0)
        return SPLIT_BLOCKS;
    
    return -1;
}

void load_image()
{
    FILE* input_file = fopen(input_filepath, "r");
    if (input_file == NULL)
    {
        fprintf(stderr, "Couldn't open input file: %s.\n", input_filepath);
        exit(1);
    }

    if (pgm_parse(input_file, &image) != PGM_SUCCESS)
    {
        fprintf(stderr, "Couldn't parse PGM image '%s': %s\n", input_filepath, pcm_get_error());
        exit(1);
    }

    fclose(input_file);
}

void run_jobs()
{
    pthread_t* threads = malloc(sizeof(pthread_t) * thread_count);
    double* thread_times = malloc(sizeof(double) * thread_count);

    // Starting to measure total time.
    struct timespec start, stop;
    if (clock_gettime(CLOCK_REALTIME, &start) == -1)
    {
        perror("Failed to get clock time.");
        exit(1);
    }

    for (int i = 0; i < thread_count; ++i)
    {
        threads[i] = create_thread(i);
    }

    // Waiting for every thread to finish.
    for (int i = 0; i < thread_count; ++i)
    {
        double* time_spent;
        pthread_join(threads[i], (void **) &time_spent);

        thread_times[i] = *time_spent;
        // Deallocating what was allocated by the thread.
        free(time_spent);
    }

    // Stopping the measure of total time.
    if (clock_gettime(CLOCK_REALTIME, &stop) == -1)
    {
        perror("Failed to get clock time.");
        exit(1);
    }

    printf(COL_YELLOW "\n=============================\n" COL_RESET);
    printf(COL_YELLOW "Total duration: " COL_RESET "%lfms\n", tu_get_elapsed_ms(&start, &stop));
    for (int i = 0; i < thread_count; ++i)
    {
        printf("  - " COL_YELLOW "Thread %03d" COL_RESET ": %lfms\n", i, thread_times[i]);
    }

    free(threads);
    free(thread_times);
}

void split_numbers(RegionSet_t* regionset, int min_value, int max_value)
{
    for (int x = 0; x < image.width; ++x)
    {
        int y_from = 0;

        for (int y = 0; y < image.height; ++y)
        {
            int pixel_idx = y * image.width + x;
            if (image.values[pixel_idx] < min_value || image.values[pixel_idx] > max_value)
            {
                // We found a pixel that doesn't match.
                regionset_add(regionset, x, x + 1, y_from, y);

                y_from = y + 1;
            }
            else if (y == image.height - 1)
            {
                // It does match, but we're at the end of the column.
                regionset_add(regionset, x, x + 1, y_from, image.height);
            }
        }
    }
}

pthread_t create_thread(int idx)
{
    pthread_t thread;

    ThreadArgs_t* thread_args = malloc(sizeof(ThreadArgs_t));
    thread_args->thread_idx = idx;
    regionset_init(&thread_args->regionset);

    if (mode == SPLIT_NUMBERS)
    {
        // Splitting the domain into value chunks
        int from = image.max_value * idx / thread_count;
        int to   = image.max_value * (idx + 1) / thread_count;

        split_numbers(&thread_args->regionset, from, to);
    }
    else
    {
        // Splitting the domain into blocks.
        int from = image.width * idx / thread_count;
        int to   = image.width * (idx + 1) / thread_count;

        regionset_add(&thread_args->regionset, from, to, 0, image.height);
    }

    if (pthread_create(&thread, NULL, thread_routine, thread_args) != 0)
    {
        perror("Failed to create thread.");
        return -1;
    }

    return thread;
}

void* thread_routine(void* args)
{
    ThreadArgs_t* thread_args = args;

    printf("[%d] START\n", thread_args->thread_idx);

    // Starting to measure time.
    struct timespec start, stop;
    if (clock_gettime(CLOCK_REALTIME, &start) == -1)
    {
        perror("Failed to get clock time.");
        exit(1);
    }

    for (int region_idx = 0; region_idx < thread_args->regionset.regions_count; ++region_idx)
    {
        Region_t* region = &thread_args->regionset.regions[region_idx];

        for (int y = region->y_from; y < region->y_to; ++y)
        {
            for (int x = region->x_from; x < region->x_to; ++x)
            {
                int index = y * image.width + x;
                // Inverting the color.
                image.values[index] = 255 - image.values[index];
            }
        }
    }

    // Stopping the measure of total time.
    if (clock_gettime(CLOCK_REALTIME, &stop) == -1)
    {
        perror("Failed to get clock time.");
        exit(1);
    }

    printf("[%d] END\n", thread_args->thread_idx);

    // Freeing the memory after use.
    free(thread_args);


    // Creating a return value.
    double* time_spent = malloc(sizeof(double));
    *time_spent = tu_get_elapsed_ms(&start, &stop);

    return time_spent;
}
