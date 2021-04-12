#ifndef SENDER_MODE_H
#define SENDER_MODE_H

enum Mode
{
    MODE_KILL,
    MODE_SIGQUEUE,
    MODE_SIGRT
};

enum Mode parse_mode(const char* mode_str);

#endif //SENDER_MODE_H
