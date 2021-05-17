#ifndef LAB9_ELF_H
#define LAB9_ELF_H

#include <pthread.h>

pthread_t create_elf_thread(int idx);
_Noreturn void* elf_thread_routine(void*);


#endif //LAB9_ELF_H
