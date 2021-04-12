#ifndef SENDER_MSG_H
#define SENDER_MSG_H

#include <stdbool.h>
#include <signal.h>

typedef void (*MessageSender_t)(int, bool, int);

void send_kill(int catcher_pid, bool finish_message, int msg_index);
void send_sigqueue(int catcher_pid, bool finish_message, int msg_index);
void send_realtime(int catcher_pid, bool finish_message, int msg_index);
void setup_sigaction(int id, void (*action)(int, siginfo_t*, void*));
void send_messages(MessageSender_t sender, int catcher_pid, int amount);

#endif //SENDER_MSG_H
