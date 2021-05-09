#ifndef ZAD1_UTILS_H
#define ZAD1_UTILS_H

#define INIT_LOG_FMT "\033[1;33m[INIT]\033[0m %-32s"
#define COL_RESET "\033[0m"
#define COL_YELLOW "\033[1;33m"
#define COL_BLUE "\033[0;34m"
#define COL_CYAN "\033[0;36m"
#define COL_ERROR "\033[0;31m"

void print_ok_msg(const char* msg, ...);
void print_ok();
void print_error();
int sscanint(const char* buffer, int* output);

#endif // ZAD1_UTILS_H