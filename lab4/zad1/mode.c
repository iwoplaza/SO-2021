#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "mode.h"

enum Mode parse_mode(const char* command)
{
    if (strcmp(command, "ignore") == 0)
    {
        return MODE_IGNORE;
    }
    else if (strcmp(command, "handler") == 0)
    {
        return MODE_HANDLER;
    }
    else if (strcmp(command, "mask") == 0)
    {
        return MODE_MASK;
    }
    else if (strcmp(command, "pending") == 0)
    {
        return MODE_PENDING;
    }

    fprintf(stderr, "Unknown mode: %s\n", command);
    exit(1);
}
