#ifndef LAB7_UTILS_H
#define LAB7_UTILS_H

#include "helper.h"

#define INIT_LOG_FMT "\033[1;33m[INIT]\033[0m %-32s"
#define COL_RESET "\033[0m"
#define COL_YELLOW "\033[1;33m"
#define COL_BLUE "\033[0;34m"
#define COL_CYAN "\033[0;36m"
#define COL_ERROR "\033[0;31m"

void print_ok_msg(const char* msg, ...);
void print_ok();
void print_error();
void mark_start_time();
void print_common_info();
void sleep_between_seconds(int from, int to);
int get_pizzas_in_oven(Helper_t* helper);
int get_pizzas_on_table(Helper_t* helper);
void print_table(int* table, int size);

#endif //LAB7_UTILS_H