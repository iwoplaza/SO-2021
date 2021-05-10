#include <time.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include "utils.h"

void print_ok_msg(const char* msg, ...)
{
    va_list args;
    va_start(args, msg);

    printf("\033[30;42m   OK  \033[0m");

    if (msg != NULL)
    {
        printf("\033[0;32m [");
        vprintf(msg, args);
        printf("\033[0m]");
    }

    printf("\n");

    va_end(args);
}

void print_ok()
{
    print_ok_msg(NULL);
}

void print_error()
{
    printf("\033[30;41m ERROR \033[0m\n");
}

static __time_t start_time;

void mark_start_time()
{
    struct timespec tms;

    clock_gettime(CLOCK_REALTIME, &tms);

    start_time = tms.tv_sec;
}

void print_common_info()
{
    struct timespec tms;

    clock_gettime(CLOCK_REALTIME, &tms);

    long long full_ms = (tms.tv_sec - start_time) * 1000;
    long delta_ms = tms.tv_nsec / 1000000;

    printf("(%d, %lld) ", getpid(), full_ms + delta_ms);
}

void sleep_between_seconds(int from, int to)
{
    int seconds = from + rand() % (to - from + 1);
    sleep(seconds);
}

int get_pizzas_in_oven(Helper_t* helper)
{
    return OVEN_CAPACITY - helper_get_sem(helper, SEM_IDX_OVEN_CAPACITY);
}

int get_pizzas_on_table(Helper_t* helper)
{
    return helper_get_sem(helper, SEM_IDX_TABLE_ITEMS);
}

void print_table(int* table, int size)
{
    printf("[");
    for (int i = 0; i < size; ++i)
    {
        printf("%d ", table[i]);
    }
    printf("]\n");
}