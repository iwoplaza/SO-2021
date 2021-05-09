#ifndef ZAD1_DEFS_H
#define ZAD1_DEFS_H

#define QUEUE_SERVER_SYMBOL 'S'

typedef enum MsgType_t {
    MSG_INIT = 1, // Client -> Server (+response).
    MSG_LIST, // Client -> Server (+response).
    MSG_CHAT, // Client -> Client.
    MSG_CONNECT, // This can be sent by the server at any time.
    MSG_DISCONNECT, // This can be sent by the server at any time.
    MSG_STOP, // This can be sent by the server at any time.
    MSG_TYPES,
} MsgType_t;

extern const char* MSG_TYPE_NAME[MSG_TYPES];

#endif //ZAD1_DEFS_H
