#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "common.h"
#include "elf.h"
#include "santa.h"
#include "raindeer.h"

static void handle_signal(int);
static void parse_arguments();
static void run_jobs();

int elf_count;
int raindeer_count;

// Shared data
int elves_in_workshop[MAX_ELFS_IN_WORKSHOP];
int elves_in_workshop_count = 0;
int raindeer_returned_count = 0;

pthread_mutex_t access_mutex = PTHREAD_MUTEX_INITIALIZER;

// Condition variables
pthread_cond_t santa_wakeup_condition = PTHREAD_COND_INITIALIZER;
pthread_cond_t elves_inspected_condition = PTHREAD_COND_INITIALIZER;
pthread_cond_t presents_delivered_condition = PTHREAD_COND_INITIALIZER;


int main(int argc, char** argv)
{
    if (argc != 3)
    {
        printf("Usage: <elf_count> <raindeer_count>");
        return 0;
    }

//    printf("SIGRTMIN %d\n", SIGRTMIN);
//    signal(SIGRTMIN + 31, handle_signal);
//    signal(SIGRTMIN + 32, handle_signal);
//    signal(SIGRTMIN + 33, handle_signal);

    // Parsing arguments.
    parse_arguments(argv);

    // Running threads
    run_jobs();

    return 0;
}

void handle_signal(int signo)
{
    printf("Handled signal %d\n", signo);
    pthread_mutex_unlock(&access_mutex);
}

void parse_arguments(char** argv)
{
    elf_count = atoi(argv[1]);
    raindeer_count = atoi(argv[2]);

    // Checking validity
    if (elf_count <= 0)
    {
        fprintf(stderr, "There has to be at least %d elves.\n", MAX_ELFS_IN_WORKSHOP);
        exit(1);
    }

    if (raindeer_count == -1)
    {
        fprintf(stderr, "There has to be at least 1 raindeer.\n");
        exit(1);
    }
}

pthread_t create_thread(int idx, void* (*routine)(void*))
{
    pthread_t thread;

    ThreadArgs_t* thread_args = malloc(sizeof(ThreadArgs_t));
    thread_args->entity_idx = idx;
    thread_args->seed = rand();

    thread_args->elf_count = elf_count;
    thread_args->raindeer_count = raindeer_count;

    thread_args->elves_in_workshop = elves_in_workshop;
    thread_args->elves_in_workshop_count = &elves_in_workshop_count;
    thread_args->raindeer_returned_count = &raindeer_returned_count;

    thread_args->access_mutex = &access_mutex;

    thread_args->santa_wakeup_condition = &santa_wakeup_condition;
    thread_args->elves_inspected_condition = &elves_inspected_condition;
    thread_args->presents_delivered_condition = &presents_delivered_condition;

    if (pthread_create(&thread, NULL, routine, thread_args) != 0)
    {
        perror("Failed to create thread.");
        return -1;
    }

    return thread;
}

void run_jobs()
{
    pthread_t* threads = malloc(sizeof(pthread_t) * (elf_count + raindeer_count));

    // Creating elves
    for (int i = 0; i < elf_count; ++i)
    {
        threads[i] = create_thread(i, elf_thread_routine);
    }

    // Creating raindeer
    for (int i = 0; i < raindeer_count; ++i)
    {
        threads[elf_count + i] = create_thread(i, raindeer_thread_routine);
    }

    // Waiting for the santa thread to finish.
    pthread_join(create_thread(0, santa_thread_routine), NULL);

    // Cancelling all threads.
    for (int i = 0; i < elf_count + raindeer_count; ++i)
    {
        pthread_cancel(threads[i]);
    }

    // Waiting for all threads to finish.
    for (int i = 0; i < elf_count + raindeer_count; ++i)
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

    printf("Finished all jobs.\n");
}
