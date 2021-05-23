#include "elf.h"
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include "utils.h"
#include "common.h"

#define LOG_ELF_PREFIX COL_YELLOW "Elf: " COL_RESET


static void elf_cleanup_handler(void* args)
{
    ThreadArgs_t* elf_args = args;

    // Unlocking possibly locked mutexes.
    pthread_mutex_unlock(elf_args->access_mutex);

    printf(LOG_ELF_PREFIX "END" LOG_INDEXED_SUFFIX, elf_args->entity_idx);

    free(args);
}

_Noreturn void* elf_thread_routine(void* args)
{
    setup_thread();

    ThreadArgs_t* thread_args = args;

    struct drand48_data rand_context;
    srand48_r(thread_args->seed, &rand_context);

    printf(LOG_ELF_PREFIX "START" LOG_INDEXED_SUFFIX, thread_args->entity_idx);

    pthread_cleanup_push(elf_cleanup_handler, args);

    while(true)
    {
        // Working
        sleep_between_seconds(&rand_context, 2, 5);

        pthread_mutex_lock(thread_args->access_mutex);

        while (*(thread_args->elves_in_workshop_count) >= MAX_ELFS_IN_WORKSHOP)
        {
            printf(LOG_ELF_PREFIX "czeka na powrót elfów" LOG_INDEXED_SUFFIX,
                   thread_args->entity_idx);

            pthread_cond_wait(thread_args->elves_inspected_condition, thread_args->access_mutex);
        }

        thread_args->elves_in_workshop[*(thread_args->elves_in_workshop_count)] = thread_args->entity_idx;
        (*thread_args->elves_in_workshop_count)++;

        printf(LOG_ELF_PREFIX "czeka %d elfów na Mikołaja" LOG_INDEXED_SUFFIX,
               *thread_args->elves_in_workshop_count, thread_args->entity_idx);

        if ((*thread_args->elves_in_workshop_count) >= MAX_ELFS_IN_WORKSHOP)
        {
            printf(LOG_ELF_PREFIX "wybudzam Mikołaja" LOG_INDEXED_SUFFIX,
                   thread_args->entity_idx);

            // Letting everyone know that a wakeup condition has been fulfilled.
            pthread_cond_broadcast(thread_args->santa_wakeup_condition);
        }

        // Waiting for the elves to be inspected, including outselves.
        pthread_cond_wait(thread_args->elves_inspected_condition, thread_args->access_mutex);

        printf(LOG_ELF_PREFIX "Mikołaj rozwiązuje problem" LOG_INDEXED_SUFFIX,
               thread_args->entity_idx);

        // Cancellating point.
        pthread_testcancel();

        // The mutex is going to be released upon cancellation, so we're
        // free to unlock it here, after the cancellation point.
        pthread_mutex_unlock(thread_args->access_mutex);
    }

    // Freeing the memory after use.
    pthread_cleanup_pop(true);
}