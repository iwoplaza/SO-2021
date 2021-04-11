#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>
#include "mode.h"

const char* USAGE = "Usage: <ignore|handler|mask|ending>\n";
const int SIGNAL_ID = SIGUSR1;

void handle_signal(int sigtype)
{
    printf("Received signal %d\n", sigtype);
}

void mask_signal()
{
    sigset_t blocked_set;
    // Emptying the set
    sigemptyset(&blocked_set);
    // Adding our signal to the set.
    sigaddset(&blocked_set, SIGNAL_ID);
    // Blocking each signal in the set (the one in our case)
    sigprocmask(SIG_BLOCK, &blocked_set, NULL);
}

void check_signal_pending()
{
    sigset_t pending_set;
    sigpending(&pending_set);

    if (sigismember(&pending_set, SIGNAL_ID))
    {
        printf("Signal with id %d is pending.\n", SIGNAL_ID);
    }
    else
    {
        printf("Signal with id %d is NOT pending.\n", SIGNAL_ID);
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("%s", USAGE);
        return 1;
    }

    const enum Mode mode = parse_mode(argv[1]);

    switch (mode)
    {
        case MODE_IGNORE:
            // We ignore the signal
            signal(SIGNAL_ID, SIG_IGN);
            break;
        case MODE_HANDLER:
            signal(SIGNAL_ID, handle_signal);
            break;
        case MODE_MASK:
        case MODE_PENDING:
            mask_signal();
            break;
        default:
            // Doing nothing.
            break;
    }

    // Raising the signal in the main process.
    printf("Raising in main process...\n");
    raise(SIGNAL_ID);

    printf("Main process: ");
    check_signal_pending();

    int pid = fork();
    if (pid == -1)
    {
        fprintf(stderr, "Failed to fork.");
        exit(1);
    }

    if (pid == 0)
    {
        // We're in the new child process.

        if (mode != MODE_PENDING)
        {
            // Raising the signal in the child process.
            printf("Raising in child process...\n");
            raise(SIGNAL_ID);
        }

        printf("Child process: ");
        check_signal_pending();

        if (execl("./other", "other", NULL) == -1)
        {
            fprintf(stderr, "Couldn't run sub-program 'other'. Make sure to compile it first.");
            exit(1);
        }
    }

    // Waiting for each child process to end.
    int status = 0;
    while (wait(&status) > 0);

    return 0;
}
