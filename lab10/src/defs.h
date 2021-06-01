#ifndef LAB10_DEFS
#define LAB10_DEFS

#define MAX_USERNAME_LENGTH 16
#define MAX_CLIENTS 16

typedef enum MessageType_t {
    MSG_PING, // Used for checking if the receiver is still alive.
    MSG_JOIN, // Client -> Server + Response
    MSG_START, // Server -> Client
    MSG_MOVE, // Client -> Server, Server -> Client (updating state of other player)
    MSG_QUIT, // Client -> Server, Server -> Client
    MSG_MAX
} MessageType_t;

typedef enum JoinStatus_t {
    JOIN_SUCCESS,
    JOIN_NAME_TAKEN,
    JOIN_ALREADY_FULL,
} JoinStatus_t;

extern char MESSAGE_TYPE_NAMES[][20];

#endif //LAB10_DEFS