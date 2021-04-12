#ifndef SENDER_MODE_H
#define SENDER_MODE_H

enum Mode
{
    MODE_KILL,
    MODE_SIGQUEUE,
    MODE_SIGRT
};

enum Mode parse_mode(const char* mode_str);
int get_regular_msg_id(enum Mode mode);
int get_finishing_msg_id(enum Mode mode);
sigset_t setup_mask(enum Mode mode);

#endif //SENDER_MODE_H
