#include <unistd.h>
#include "utils.h"

double get_random_double(struct drand48_data* context)
{
    double random_value;

    drand48_r(context, &random_value);

    return random_value;
}

void sleep_between_seconds(struct drand48_data* context, double from, double to)
{
    double seconds = from + get_random_double(context) * (to - from + 1);
    usleep(seconds * 1e06);
}