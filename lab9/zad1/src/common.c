#include "common.h"
#include <signal.h>
#include <stdio.h>

void setup_thread()
{
    sigset_t set;
    sigemptyset(&set);

//    if (pthread_sigmask(SIG_SETMASK, &set, NULL) != 0)
//    {
//        perror("Couldn't setup thread signal mask");
//    }
}
