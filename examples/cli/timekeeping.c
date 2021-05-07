#include "timekeeping.h"

#include <stdio.h>

void print_time(const CLIContext_t* ctx, const char* label, int lines, clock_t delta)
{
    double miliseconds = (double) delta / CLOCKS_PER_SEC * 1000;
    printf("TIME: %-20s %10ld ticks, %10f ms\n", label, delta, miliseconds);

    char buffer[256];
    sprintf(buffer, "%s (%d lines)", label, lines);

    if (ctx->report_file != NULL)
    {
        fprintf(ctx->report_file, "%-32s %10ld ticks, %10f ms\n", buffer, delta, miliseconds);
    }
}