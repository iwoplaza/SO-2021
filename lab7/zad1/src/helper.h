#ifndef LAB7_HELPER_H
#define LAB7_HELPER_H

#include <stdbool.h>
#include "defs.h"

#ifdef LAB7_SYSTEMV
    #include "helper_systemv.h"
#elif LAB7_POSIX
    #include "helper_posix.h"
#else
    typedef struct Helper_t {} Helper_t;
#endif

typedef enum SemIdx_t
{
    // Capacity of the oven
    SEM_IDX_OVEN_CAPACITY,
    // Capacity of the table
    SEM_IDX_TABLE_CAPACITY,
    // Oven accessibility
    SEM_IDX_OVEN_ACCESS,
    // Table accessibility
    SEM_IDX_TABLE_ACCESS,
    // The amount of items on the table
    SEM_IDX_TABLE_ITEMS,
    // The amount of semaphores, will automatically be calculated.
    SEM_AMOUNT
} SemIdx_t;

Helper_t* helper_init(bool setup);

void helper_destroy(Helper_t* helper);

void helper_free(Helper_t* helper);

void helper_sem_change(Helper_t* helper, int sem_idx, short delta);

void helper_request_access(Helper_t* helper, int sem_idx);

void helper_return_access(Helper_t* helper, int sem_idx);

int helper_get_sem(Helper_t* helper, int sem_idx);

SharedMemoryData_t* helper_get_shared_data(Helper_t* helper);

void helper_set_interrupt_handler(void (*handler)());

const char* helper_get_error();

#endif //LAB7_HELPER_H