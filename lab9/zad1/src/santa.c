#include "santa.h"
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include "utils.h"
#include "common.h"

#define LOG_SANTA_PREFIX COL_YELLOW "Santa: " COL_RESET


static void santa_cleanup_handler(void* args)
{
    ThreadArgs_t* thread_args = args;

    // Unlocking possibly locked mutexes.
    pthread_mutex_unlock(thread_args->elves_in_workshop_mutex);

    printf(LOG_SANTA_PREFIX "END" LOG_DEFAULT_SUFFIX);

    free(args);
}

_Noreturn void* santa_thread_routine(void* args)
{
    ThreadArgs_t* thread_args = args;

    struct drand48_data rand_context;
    srand48_r(thread_args->seed, &rand_context);

    printf(LOG_SANTA_PREFIX "START" LOG_DEFAULT_SUFFIX);

    pthread_cleanup_push(santa_cleanup_handler, args);

    while(true)
    {
        // Sleeping and waiting for instruction.
        while (
            *(thread_args->elves_in_workshop_count) < MAX_ELFS_IN_WORKSHOP &&
            *(thread_args->raindeer_returned_count) < *(thread_args->raindeer_count)
        ) {
            printf(LOG_SANTA_PREFIX "czeka na powrót elfów" LOG_DEFAULT_SUFFIX);

            pthread_cond_wait(thread_args->santa_wakeup_condition, thread_args->elves_in_workshop_mutex);
        }

        thread_args->elves_in_workshop[*(thread_args->elves_in_workshop_count)] = thread_args->entity_idx;
        (*thread_args->elves_in_workshop_count)++;

        printf(LOG_ELF_PREFIX "czeka %d elfów na Mikołaja" LOG_INDEXED_SUFFIX,
                *thread_args->elves_in_workshop_count, thread_args->entity_idx);

        if ((*thread_args->elves_in_workshop_count) >= 3)
        {
            printf(LOG_SANTA_PREFIX "wybudzam Mikołaja" LOG_DEFAULT_SUFFIX);

            // Letting everyone know that a wakeup condition has been fulfilled.
            pthread_cond_broadcast(thread_args->santa_wakeup_condition);
        }

        // Waiting for the elves to be inspected, including outselves.
        pthread_cond_wait(thread_args->elves_inspected_condition, thread_args->elves_in_workshop_mutex);

        printf(LOG_ELF_PREFIX "Mikołaj rozwiązuje problem" LOG_INDEXED_SUFFIX,
                thread_args->entity_idx);

        // Cancellating point.
        pthread_testcancel();

        // The mutex is going to be released upon cancellation, so we're
        // free to unlock it here, after the cancellation point.
        pthread_mutex_unlock(thread_args->elves_in_workshop_mutex);
    }

    // Freeing the memory after use.
    pthread_cleanup_pop(true);
}