#ifndef LAB7_DEFS_H
#define LAB7_DEFS_H

#define OVEN_CAPACITY 5
#define TABLE_CAPACITY 5

#define IPC_KEY_PROJECT 'H'

typedef struct SharedMemoryData_t {
    int oven[OVEN_CAPACITY];
    int table[TABLE_CAPACITY];
    int oven_tail;
    int table_tail;
} SharedMemoryData_t;

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

#endif //LAB7_DEFS_H