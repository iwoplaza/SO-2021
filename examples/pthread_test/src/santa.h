#ifndef LAB9_SANTA_H
#define LAB9_SANTA_H

#include <pthread.h>

pthread_t create_santa_thread(int idx);
_Noreturn void* santa_thread_routine(void*);


#endif //LAB9_SANTA_H
