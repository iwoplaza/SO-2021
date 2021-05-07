#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

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

/**
 * Works like sscanf for a single integer, but returns the amount of
 * buffer that was "consumed" to parse that int.
 * @param buffer
 * @param output
 * @return
 */
int sscanint(const char* buffer, int* output)
{
    static char read_buffer[64];

    sscanf(buffer, "%s", read_buffer);

    *output = atoi(read_buffer);

    return strlen(read_buffer);
}