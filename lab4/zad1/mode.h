#ifndef ZAD1_MODE_H
#define ZAD1_MODE_H

enum Mode
{
    MODE_IGNORE,
    MODE_HANDLER,
    MODE_MASK,
    MODE_PENDING
};

enum Mode parse_mode(const char* command);

#endif //ZAD1_MODE_H
