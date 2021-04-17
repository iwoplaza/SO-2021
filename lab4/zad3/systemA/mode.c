#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
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

int get_regular_msg_id(enum Mode mode)
{
    return mode == MODE_SIGRT ? SIGRTMIN : SIGUSR1;
}

int get_finishing_msg_id(enum Mode mode)
{
    return mode == MODE_SIGRT ? (SIGRTMIN + 1) : SIGUSR2;
}

sigset_t setup_mask(enum Mode mode)
{
    sigset_t mask;
    sigfillset(&mask);

    sigdelset(&mask, get_regular_msg_id(mode));
    sigdelset(&mask, get_finishing_msg_id(mode));

    return mask;
}