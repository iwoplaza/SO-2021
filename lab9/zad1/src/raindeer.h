#ifndef LAB9_RAINDEER_H
#define LAB9_RAINDEER_H

#include <pthread.h>

pthread_t create_raindeer_thread(int idx);
_Noreturn void* raindeer_thread_routine(void*);

#endif //LAB9_RAINDEER_H
