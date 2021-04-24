#ifndef AGGREGATE_H
#define AGGREGATE_H

#include <stdbool.h>

typedef struct AggregateLine_t {
    char* value;
    int capacity;
    int count;
} AggregateLine_t;

typedef struct Aggregate_t {
    AggregateLine_t* lines;
    int capacity;
    int count;
} Aggregate_t;

void aggregate_add_line(Aggregate_t* aggregate);

void aggregate_add_data(Aggregate_t* aggregate, int line_idx, char* data, int data_size);

Aggregate_t* read_aggregate(int file);

void write_aggregate(Aggregate_t* aggregate, int file);

void print_aggregate(Aggregate_t* aggregate);

void free_aggregate(Aggregate_t* aggregate);

bool cmp_aggregate_line(Aggregate_t* a, int line_idx, const char* value);

#endif // AGGREGATE_H
