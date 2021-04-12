#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include "mode.h"
#include "msg.h"

const char* USAGE = "Usage: <KILL|SIGQUEUE|SIGRT>\n";

pid_t sender_pid;
bool waiting_for_responses = true;
int signals_received = 0;

void send_messages(MessageSender_t sender, int catcher_pid, int amount)
{
    for (int i = 0; i < amount; ++i)
    {
        sender(catcher_pid, false, i);
    }

    sender(catcher_pid, true, amount);
}

void signal_handler(int sig, siginfo_t* info, void* ucontext)
{
    if (sig == SIGUSR2 || sig == SIGRTMIN + 1)
    {
        sender_pid = info->si_pid;
        waiting_for_responses = false;
        printf("Finished listening to signals. Received %d total. \n", signals_received);
        return;
    }

    printf("Got signal with id: %d\n", info->si_value.sival_int);
    signals_received++;
}

int get_regular_msg_id(enum Mode mode)
{
    return mode == MODE_SIGRT ? SIGRTMIN : SIGUSR1;
}

int get_finishing_msg_id(enum Mode mode)
{
    return mode == MODE_SIGRT ? SIGRTMIN + 1 : SIGUSR2;
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

    // Wait for responses.
    sigset_t mask;
    sigfillset(&mask);

    switch (mode)
    {
        case MODE_KILL:
        case MODE_SIGQUEUE:
            sigdelset(&mask, SIGUSR1);
            sigdelset(&mask, SIGUSR2);
            break;
        case MODE_SIGRT:
            sigdelset(&mask, SIGRTMIN);
            sigdelset(&mask, SIGRTMIN + 1);
            break;
        default:
            break;
    }

    while (waiting_for_responses)
    {
        sigsuspend(&mask);
    }

    send_messages(sender, sender_pid, signals_received);

    return 0;
}
