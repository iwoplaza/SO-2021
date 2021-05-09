#include <time.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>

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

void print_common_info()
{
    struct timespec tms;

    clock_gettime(CLOCK_REALTIME, &tms);

    long ms = round(tms.tv_nsec / 1.0e6);

    printf("(%d, %ld) ", getpid(), ms);
}

void sleep_between_seconds(int from, int to)
{
    sleep(from + rand() % (to - from + 1));
}