#ifndef LAB9_COMMON_H
#define LAB9_COMMON_H

#include <pthread.h>

#define MAX_ELFS_IN_WORKSHOP 3

#define COL_RESET   "\033[0m"
#define COL_YELLOW  "\033[1;33m"
#define COL_BLUE    "\033[0;34m"
#define COL_CYAN    "\033[0;36m"
#define COL_ERROR   "\033[0;31m"

#define LOG_INDEXED_SUFFIX      ", " COL_CYAN "[%d]\n" COL_RESET
#define LOG_DEFAULT_SUFFIX      COL_RESET

typedef struct ThreadArgs_t {
    int entity_idx;
    long seed;

    int elf_count;
    int raindeer_count;

    // Critical Section: Elves In Workshop
    int* elves_in_workshop;
    int* elves_in_workshop_count;
    pthread_mutex_t* elves_in_workshop_mutex;

    // Critical Section: Returned Raindeer
    int* raindeer_returned_count;
    pthread_mutex_t* raindeer_returned_mutex;

    // Condition variables
    pthread_cond_t* santa_wakeup_condition;
    pthread_cond_t* elves_inspected_condition;
    pthread_cond_t* presents_delivered_condition;
} ThreadArgs_t;

#endif //LAB9_COMMON_H
