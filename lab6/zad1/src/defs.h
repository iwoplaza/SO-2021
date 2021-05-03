#ifndef ZAD1_DEFS_H
#define ZAD1_DEFS_H

#define QUEUE_FILEPATH "/media/sf_Shared/sysopy/lab6/zad1/src/defs.h"

typedef enum MsgType_t {
    MSG_INIT = 1,
    MSG_LIST,
    MSG_CONNECT,
    MSG_DISCONNECT,
    MSG_STOP,
    MSG_TYPES,
} MsgType_t;

extern const char* MSG_TYPE_NAME[MSG_TYPES];

#endif //ZAD1_DEFS_H
