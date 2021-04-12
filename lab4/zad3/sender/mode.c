#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "mode.h"

enum Mode parse_mode(const char* mode_str)
{
    if (strcmp(mode_str, "KILL") == 0)
    {
        return MODE_KILL;
    }
    else if (strcmp(mode_str, "SIGQUEUE") == 0)
    {
        return MODE_SIGQUEUE;
    }
    else if (strcmp(mode_str, "SIGRT") == 0)
    {
        return MODE_SIGRT;
    }

    fprintf(stderr, "Unknown mode: %s\n", mode_str);
    exit(1);
}