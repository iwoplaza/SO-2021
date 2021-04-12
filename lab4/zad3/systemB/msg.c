#include "msg.h"
#include <stdio.h>
#include <stdlib.h>

void send_kill(int catcher_pid, bool finish_message, int msg_index)
{
    if (kill(catcher_pid, finish_message ? SIGUSR2 : SIGUSR1) == -1)
    {
        fprintf(stderr, "Failed to send message.\n");
        exit(1);
    }
}

void send_sigqueue(int catcher_pid, bool finish_message, int msg_index)
{
    sigval_t val = { msg_index };

    if (sigqueue(catcher_pid, finish_message ? SIGUSR2 : SIGUSR1, val) == -1)
    {
        fprintf(stderr, "Failed to send message.\n");
        exit(1);
    }
}

void send_realtime(int catcher_pid, bool finish_message, int msg_index)
{
    if (kill(catcher_pid, finish_message ? (SIGRTMIN + 1) : SIGRTMIN) == -1)
    {
        fprintf(stderr, "Failed to send message.\n");
        exit(1);
    }
}

void setup_sigaction(int id, void (*action)(int, siginfo_t*, void*))
{
    static struct sigaction act;
    act.sa_sigaction = action;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);

    sigaction(id, &act, NULL);
}

void send_messages(MessageSender_t sender, int catcher_pid, int amount)
{
    printf("Sending %d messages to %d\n", amount, catcher_pid);

    for (int i = 0; i < amount; ++i)
    {
        sender(catcher_pid, false, i);
    }

    sender(catcher_pid, true, amount);
}