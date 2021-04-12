#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include "mode.h"
#include "msg.h"

const char* USAGE = "Usage: <KILL|SIGQUEUE|SIGRT>\n";

MessageSender_t sender = NULL;
bool waiting_for_responses = true;
int signals_received = 0;

void signal_handler(int sig, siginfo_t* info, void* ucontext)
{
    int sender_pid = info->si_pid;

    if (sig == SIGUSR2 || sig == SIGRTMIN + 1)
    {
        waiting_for_responses = false;
        sender(sender_pid, true, signals_received);
        printf("Finished listening to signals. Received %d total. \n", signals_received);
        return;
    }

    printf("Got signal with id: %d\n", info->si_value.sival_int);
    sender(sender_pid, false, signals_received);

    signals_received++;
}

int main(int argc, char** argv)
{
    printf("==CATCHER==\n");
    printf("==========\n\n");

    if (argc != 2)
    {
        printf("%s", USAGE);
        return 1;
    }

    printf("PID: %d\n", getpid());

    enum Mode mode = parse_mode(argv[1]);

    sender = NULL;
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

    // Wait for messages.
    sigset_t mask = setup_mask(mode);

    printf("Waiting for signals...");
    while (waiting_for_responses)
    {
        sigsuspend(&mask);
    }

    return 0;
}
