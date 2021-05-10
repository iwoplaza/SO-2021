#include "timeutil.h"

double tu_get_elapsed_ms(struct timespec* const start, struct timespec* const stop)
{
    return ((stop->tv_sec - start->tv_sec) * 1000000 + (stop->tv_nsec - start->tv_nsec) / 1000) / 1.0e3;
}