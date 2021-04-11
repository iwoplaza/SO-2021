#include <signal.h>
#include <stdio.h>

const int SIGNAL_ID = SIGUSR1;

void check_signal_pending()
{
    sigset_t pending_set;
    sigpending(&pending_set);

    if (sigismember(&pending_set, SIGNAL_ID))
    {
        printf("After exec, signal with id %d is pending.\n", SIGNAL_ID);
    }
    else
    {
        printf("After exec, signal with id %d is NOT pending.\n", SIGNAL_ID);
    }
}

int main(int argc, char** argv)
{
    check_signal_pending();

    raise(SIGNAL_ID);

    return 0;
}