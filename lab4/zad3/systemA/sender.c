#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mode.h"
#include "msg.h"

const char* USAGE = "Usage: <catcher_pid> <signal_count> <KILL|SIGQUEUE|SIGRT>\n";

bool waiting_for_responses = true;
int signals_to_send;
int signals_received = 0;

void signal_handler(int sig, siginfo_t* info, void* ucontext)
{
    if (sig == SIGUSR2 || sig == SIGRTMIN + 1)
    {
        waiting_for_responses = false;
        printf("Finished listening to signals. Received %d out of %d expected.\n", signals_received, signals_to_send);
        return;
    }

    signals_received++;
    printf("Got signal with id: %d\n", info->si_value.sival_int);
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
    signals_to_send = atoi(argv[2]);
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

    send_messages(sender, catcher_pid, signals_to_send);

    // Wait for responses.
    sigset_t mask = setup_mask(mode);

    while (waiting_for_responses)
    {
        sigsuspend(&mask);
    }

    return 0;
}
