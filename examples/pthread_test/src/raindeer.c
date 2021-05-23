#include "raindeer.h"
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include "utils.h"
#include "common.h"

#define LOG_RAINDEER_PREFIX COL_YELLOW "Renifer: " COL_RESET


void raindeer_cleanup_handler(void* args)
{
    ThreadArgs_t* thread_args = args;

    // Unlocking possibly locked mutexes.
    pthread_mutex_unlock(thread_args->access_mutex);

    printf(LOG_RAINDEER_PREFIX "END" LOG_INDEXED_SUFFIX, thread_args->entity_idx);

    free(args);
}

_Noreturn void* raindeer_thread_routine(void* args)
{
    setup_thread();

    ThreadArgs_t* thread_args = args;

    struct drand48_data rand_context;
    srand48_r(thread_args->seed, &rand_context);

    printf(LOG_RAINDEER_PREFIX "START" LOG_INDEXED_SUFFIX, thread_args->entity_idx);

    pthread_cleanup_push(raindeer_cleanup_handler, args);

    while(true)
    {
        // Going to vacation
        sleep_between_seconds(&rand_context, 5, 10);

        pthread_mutex_lock(thread_args->access_mutex);

        // We returned, so we increase the count.
        (*thread_args->raindeer_returned_count)++;

        printf(LOG_RAINDEER_PREFIX "czeka %d reniferów na Mikołaja" LOG_INDEXED_SUFFIX,
               *thread_args->raindeer_returned_count, thread_args->entity_idx);

        if (*(thread_args->raindeer_returned_count) >= thread_args->raindeer_count)
        {
            printf(LOG_RAINDEER_PREFIX "wybudzam Mikołaja" LOG_INDEXED_SUFFIX,
                   thread_args->entity_idx);

            pthread_cond_broadcast(thread_args->santa_wakeup_condition);
        }

        // Waiting for the presents to be delivered by santa.
        pthread_cond_wait(thread_args->presents_delivered_condition, thread_args->access_mutex);

        // Cancellating point.
        pthread_testcancel();

        // The mutex is going to be released upon cancellation, so we're
        // free to unlock it here, after the cancellation point.
        pthread_mutex_unlock(thread_args->access_mutex);
    }

    // Freeing the memory after use.
    pthread_cleanup_pop(true);

    return NULL;
}