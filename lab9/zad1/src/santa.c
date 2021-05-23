#include "santa.h"
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include "utils.h"
#include "common.h"

#define LOG_SANTA_PREFIX COL_YELLOW "Mikołaj: " COL_RESET


static void santa_cleanup_handler(void* args)
{
    ThreadArgs_t* thread_args = args;

    // Unlocking possibly locked mutexes.
    pthread_mutex_unlock(thread_args->access_mutex);

    printf(LOG_SANTA_PREFIX "END" LOG_DEFAULT_SUFFIX);

    free(args);
}

_Noreturn void* santa_thread_routine(void* args)
{
    setup_thread();

    ThreadArgs_t* thread_args = args;

    struct drand48_data rand_context;
    srand48_r(thread_args->seed, &rand_context);

    printf(LOG_SANTA_PREFIX "START" LOG_DEFAULT_SUFFIX);

    int trips_count = 0;

    pthread_cleanup_push(santa_cleanup_handler, args);

    while(true)
    {
        // Sleeping and waiting for instruction.
        while (
            *(thread_args->elves_in_workshop_count) < MAX_ELFS_IN_WORKSHOP &&
            *(thread_args->raindeer_returned_count) < thread_args->raindeer_count
        ) {
            pthread_cond_wait(thread_args->santa_wakeup_condition, thread_args->access_mutex);
        }

        printf(LOG_SANTA_PREFIX "budzę się" LOG_DEFAULT_SUFFIX);

        if (*(thread_args->raindeer_returned_count) >= thread_args->raindeer_count)
        {
            printf(LOG_SANTA_PREFIX "dostarczam zabawki" LOG_DEFAULT_SUFFIX);
            sleep_between_seconds(&rand_context, 2, 4);

            *thread_args->raindeer_returned_count = 0;

            // Letting everyone know that presents have been delivered.
            pthread_cond_broadcast(thread_args->presents_delivered_condition);

            trips_count++;

            if (trips_count >= 3)
            {
                break;
            }
        }

        if (*(thread_args->elves_in_workshop_count) >= MAX_ELFS_IN_WORKSHOP)
        {
            printf(LOG_SANTA_PREFIX "rozwiązuje problemy elfów");
            for (int i = 0; i < MAX_ELFS_IN_WORKSHOP; ++i)
            {
                printf(" %d", thread_args->elves_in_workshop[i]);
            }
            printf(LOG_DEFAULT_SUFFIX);
            sleep_between_seconds(&rand_context, 1, 2);

            *thread_args->elves_in_workshop_count = 0;

            // Letting everyone know that elves have been inspected.
            pthread_cond_broadcast(thread_args->elves_inspected_condition);
        }

        printf(LOG_SANTA_PREFIX "zasypiam" LOG_DEFAULT_SUFFIX);

        // Cancellating point.
        pthread_testcancel();

        // The mutex is going to be released upon cancellation, so we're
        // free to unlock it here, after the cancellation point.
        pthread_mutex_unlock(thread_args->access_mutex);
    }

    // Freeing the memory after use.
    pthread_cleanup_pop(true);
}