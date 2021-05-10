#include "helper_posix.h"
#include "helper.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#define LAB7_SEM_NAME_FORMAT "/lab7-%d"
#define LAB7_SHM_NAME "/lab7-shm"

void _open_sem(Helper_t* helper, SemIdx_t idx, int oflag, mode_t mode, unsigned int value)
{
    static char name_buffer[256];
    sprintf(name_buffer, LAB7_SEM_NAME_FORMAT, idx);

    helper->semaphores[idx] = sem_open(name_buffer, oflag, mode, value);
    if (helper->semaphores[idx] == SEM_FAILED)
    {
        fprintf(stderr, "Failed to open semaphore '%s'\n", name_buffer);
        perror("Failed to open semaphore");
        return;
    }
}

Helper_t* helper_init(bool setup)
{
    Helper_t* helper = malloc(sizeof(Helper_t));

    // --SEMAPHORES--
    int oflag = setup ? (O_CREAT) : 0;
    mode_t mode = setup ? (S_IRWXU | S_IRWXG | S_IRWXO) : 0;

    _open_sem(helper, SEM_IDX_OVEN_CAPACITY, oflag, mode, OVEN_CAPACITY);
    _open_sem(helper, SEM_IDX_TABLE_CAPACITY, oflag, mode, TABLE_CAPACITY);
    _open_sem(helper, SEM_IDX_OVEN_ACCESS, oflag, mode, 1);
    _open_sem(helper, SEM_IDX_TABLE_ACCESS, oflag, mode, 1);
    _open_sem(helper, SEM_IDX_TABLE_ITEMS, oflag, mode, 0);

    // --SHARED MEMORY--

    int shm_id = shm_open(LAB7_SHM_NAME, (setup ? O_CREAT : 0) | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);

    if (shm_id == -1)
    {
        perror("Failed to open shared memory chunk.");
        free(helper);
        return NULL;
    }

    // Resizing the shared memory chunk.
    if (ftruncate(shm_id, sizeof(SharedMemoryData_t)) == -1)
    {
        perror("Failed to resize shared memory chunk.");
        free(helper);
        return NULL;
    }

    SharedMemoryData_t* shared_data = mmap(NULL, sizeof(SharedMemoryData_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
    if (shared_data == (void*)-1)
    {
        perror("Failed to link shared memory chunk.");
        free(helper);
        return NULL;
    }

    if (setup)
    {
        // Initializing shared memory.
        shared_data->oven_tail = 0;
        shared_data->table_tail = 0;

        for (int i = 0; i < OVEN_CAPACITY; ++i)
            shared_data->oven[i] = -1;

        for (int i = 0; i < TABLE_CAPACITY; ++i)
            shared_data->table[i] = -1;
    }

    helper->shm_id = shm_id;
    helper->shared_data = shared_data;

    return helper;
}

void helper_destroy(Helper_t* helper)
{
    static char name_buffer[256];

    for (int i = 0; i < SEM_AMOUNT; ++i)
    {
        sprintf(name_buffer, LAB7_SEM_NAME_FORMAT, i);

        if (sem_unlink(name_buffer) == -1)
        {
            fprintf(stderr, "Failed to unlink semaphor '%s'\n", name_buffer);
            perror("Failed to unlink semaphor");
        }
    }

    // Unlinking the shared memory chunk.
    // It will get removed once every process stop using it.
    if (shm_unlink(LAB7_SHM_NAME) == -1)
    {
        perror("Failed to unlink shared memory chunk");
    }
}

void helper_free(Helper_t* helper)
{
    // --SEMAPHORES--
    for (int i = 0; i < SEM_AMOUNT; ++i)
    {
        if (sem_close(helper->semaphores[i]) == -1)
        {
            fprintf(stderr, "Failed to close semaphor '%d'\n", i);
            perror("Failed to close semaphor");
        }
    }

    // --SHARED MEMORY--
    // Detaching shared memory chunk.
    if (munmap(helper->shared_data, sizeof(SharedMemoryData_t)) == -1)
    {
        perror("Failed to detach shared memory chunk");
    }

    free(helper);
}

void helper_sem_change(Helper_t* helper, int sem_idx, short delta)
{
    if (delta == 1)
    {
        sem_post(helper->semaphores[sem_idx]);
    }
    else if (delta == -1)
    {
        sem_wait(helper->semaphores[sem_idx]);
    }
    else
    {
        printf("Unsupported semaphor value change delta: %d\n", delta);
    }
}

void helper_request_access(Helper_t* helper, int sem_idx)
{
    helper_sem_change(helper, sem_idx, -1);
}

void helper_return_access(Helper_t* helper, int sem_idx)
{
    helper_sem_change(helper, sem_idx, 1);
}

int helper_get_sem(Helper_t* helper, int sem_idx)
{
    int value;
    
    if (sem_getvalue(helper->semaphores[sem_idx], &value) == -1)
    {
        perror("Failed to read semaphor value");
        return -1;
    }

    return value;
}

SharedMemoryData_t* helper_get_shared_data(Helper_t* helper)
{
    return helper->shared_data;
}

