#ifndef KOL1_TIMEKEEPING_H
#define KOL1_TIMEKEEPING_H

#include <time.h>
#include "cli_context.h"

void print_time(const CLIContext_t* ctx, const char* label, int lines, clock_t delta);

#endif // KOL1_TIMEKEEPING_H
