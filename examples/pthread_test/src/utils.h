#ifndef LAB9_UTILS_H
#define LAB9_UTILS_H

#include <stdlib.h>

double get_random_double(struct drand48_data* context);

void sleep_between_seconds(struct drand48_data* context, int from, int to);

#endif //LAB9_UTILS_H
