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

#endif //LAB7_DEFS_H