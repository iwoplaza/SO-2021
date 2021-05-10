#ifndef TIMEUTIL_H
#define TIMEUTIL_H

#include <time.h>
#include <unistd.h>

double tu_get_elapsed_ms(struct timespec* start, struct timespec* stop);

#endif //TIMEUTIL_H
