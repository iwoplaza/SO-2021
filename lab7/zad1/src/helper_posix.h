#ifndef LAB7_HELPER_POSIX_H
#define LAB7_HELPER_POSIX_H

#include "defs.h"
#include <semaphore.h>

typedef struct Helper_t {
    sem_t* semaphores[SEM_AMOUNT];
    int shm_id;
    SharedMemoryData_t* shared_data;
} Helper_t;

#endif //LAB7_HELPER_POSIX_H