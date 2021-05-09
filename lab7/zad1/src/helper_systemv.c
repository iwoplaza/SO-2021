#define LAB7_SYSTEMV

#include "helper.h"
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>

static char msg_error[256];

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};

void _set_sem_value(int sem_group_id, int sem_idx, int value)
{
    union semun arg;
    arg.val = value;

    semctl(sem_group_id, sem_idx, SETVAL, arg);
}

Helper_t* helper_init(bool setup)
{
    key_t key = ftok(getenv("HOME"), IPC_KEY_PROJECT);

    // --SEMAPHORES--

    int sem_group_id = -1;

    if (setup)
    {
        sem_group_id = semget(key, SEM_AMOUNT, IPC_CREAT | S_IRWXU | S_IRWXG | S_IRWXO);

        // Setting defaults
        _set_sem_value(sem_group_id, SEM_IDX_OVEN_CAPACITY, OVEN_CAPACITY);
        _set_sem_value(sem_group_id, SEM_IDX_TABLE_CAPACITY, TABLE_CAPACITY);
        _set_sem_value(sem_group_id, SEM_IDX_OVEN_ACCESS, 2);
        _set_sem_value(sem_group_id, SEM_IDX_TABLE_ACCESS, 2);
    }
    else
    {
        sem_group_id = semget(key, 0, 0);
    }

    // --SHARED MEMORY--

    int shm_id = -1;
    if (setup)
    {
        shm_id = shmget(key, sizeof(SharedMemoryData_t), IPC_CREAT | S_IRWXU | S_IRWXG | S_IRWXO);
    }
    else
    {
        shm_id = shmget(key, 0, 0);
    }

    if (shm_id == -1)
    {
        perror("Failed to open shared memory chunk.");
        return NULL;
    }

    SharedMemoryData_t* shared_data = shmat(shm_id, NULL, 0);
    if (shared_data == (void*)-1)
    {
        perror("Failed to link shared memory chunk.");
        return NULL;
    }

    Helper_t* helper = malloc(sizeof(Helper_t));
    helper->sem_group_id = sem_group_id;
    helper->shm_id = shm_id;
    helper->shared_data = shared_data;

    return helper;
}

void helper_destroy(Helper_t* helper)
{
    // Unlinking the shared memory chunk.
    // It will get removed once every process stop using it.
    shmctl(helper->shm_id, IPC_RMID, NULL);
}

void helper_free(Helper_t* helper)
{
    // --SEMAPHORES--

    // --SHARED MEMORY--

    // Detaching shared memory chunk.
    shmdt(helper->shared_data);

    free(helper);
}

void helper_sem_change(Helper_t* helper, int sem_idx, short delta)
{
    static struct sembuf sops[1];

    sops[0].sem_num = sem_idx;
    sops[0].sem_op = delta;
    sops[0].sem_flg = 0;

    semop(helper->sem_group_id, sops, 1);
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
    return semctl(helper->sem_group_id, sem_idx, GETVAL);
}

SharedMemoryData_t* helper_get_shared_data(Helper_t* helper)
{
    return helper->shared_data;
}

const char* helper_get_error()
{
    return msg_error;
}