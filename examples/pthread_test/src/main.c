#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "common.h"

static void run_jobs();

static int thread_count = 5;

void thread_cleanup_handler(void* args)
{
    printf("Cleaning up...\n");
    free(args);
}

void* thread_routine(void* args)
{
    int thread_index = *((int*) args);

    printf("THREAD %d\n", thread_index);

    pthread_cleanup_push(thread_cleanup_handler, args);

    // Sleeping
    sleep(1 + thread_index);

    // Freeing the memory after use.
    pthread_cleanup_pop(true);

    return NULL;
}

int main(int argc, char** argv)
{
    // Running threads
    run_jobs();

    return 0;
}

pthread_t create_thread(int idx, void* (*routine)(void*))
{
    pthread_t thread;

    int* args = malloc(sizeof(int));

    *args = idx;

    if (pthread_create(&thread, NULL, routine, args) != 0)
    {
        perror("Failed to create thread.");
        return -1;
    }

    return thread;
}

void run_jobs()
{
    pthread_t* threads = malloc(sizeof(pthread_t) * (thread_count));

    // Creating thread
    for (int i = 0; i < thread_count; ++i)
    {
        threads[i] = create_thread(i, thread_routine);
    }

    // Sleeping before cancelling
    sleep(2);

     // Cancelling all threads.
     for (int i = 0; i < thread_count; ++i)
     {
         printf("Trying to cancel %d\n", i);
         fflush(stdout);

         pthread_cancel(threads[i]);
     }

    // Waiting for all threads to finish.
    for (int i = 0; i < thread_count; ++i)
    {
        void* retval = NULL;

        if (pthread_join(threads[i], &retval) != 0)
        {
            perror("Failed to join with worker thread %d.");
        }
        else if (retval != PTHREAD_CANCELED)
        {
            fprintf(stderr, "The thread didn't finish by cancelling.\n");
        }
    }
}
