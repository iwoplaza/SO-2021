#include <stdio.h>
#include <stdarg.h>

void print_ok_msg(const char* msg, ...)
{
    va_list args;
    va_start(args, msg);

    printf("\033[30;42m OK ");

    if (msg != NULL)
    {
        vprintf(msg, args);
    }

    printf("\033[0m\n");

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