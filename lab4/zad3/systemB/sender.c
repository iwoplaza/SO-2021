#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mode.h"
#include "msg.h"

const char* USAGE = "Usage: <catcher_pid> <signal_count> <KILL|SIGQUEUE|SIGRT>\n";

void signal_handler(int sig, siginfo_t* info, void* ucontext)
{
    printf("Got acknowledgment.\n");
}

int main(int argc, char** argv)
{
    printf("==SENDER==\n");
    printf("==========\n\n");

    if (argc != 4)
    {
        printf("%s", USAGE);
        return 1;
    }

    int catcher_pid = atoi(argv[1]);
    int signals_to_send = atoi(argv[2]);
    enum Mode mode = parse_mode(argv[3]);

    MessageSender_t sender = NULL;
    switch (mode)
    {
        case MODE_KILL:
            sender = send_kill;
            break;
        case MODE_SIGQUEUE:
            sender = send_sigqueue;
            break;
        case MODE_SIGRT:
            sender = send_realtime;
            break;
        default:
            break;
    }

    setup_sigaction(get_regular_msg_id(mode), signal_handler);
    setup_sigaction(get_finishing_msg_id(mode), signal_handler);

    // This is the mask that will be used during normal execution.
    // It's going to block every signal, meaning that there will
    // be no signal handled between sending a signal, and setting up
    // to wait for the response.
    sigset_t default_mask;
    sigfillset(&default_mask);
    sigprocmask(SIG_SETMASK, &default_mask, NULL);

    sigset_t mask = setup_mask(mode);

    printf("Sending %d messages to %d\n", signals_to_send, catcher_pid);
    for (int i = 0; i < signals_to_send; ++i)
    {
        sender(catcher_pid, false, i);

        // Wait for response.
        sigsuspend(&mask);
    }

    // Sending last message
    sender(catcher_pid, true, signals_to_send);

    return 0;
}
