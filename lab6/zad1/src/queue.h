#ifndef ZAD1_QUEUE_H
#define ZAD1_QUEUE_H

#include <stdbool.h>
#include "defs.h"

#define MAX_MSG_LENGTH 128

typedef struct Data_t {
    long type;
    char buffer[MAX_MSG_LENGTH];
} Data_t;

typedef struct MsgQueue_t {
    int id; // System V & Posix
    char* filepath; // Posix
} MsgQueue_t;

typedef void (*MessageHandler_t)(Data_t* data);

MsgQueue_t* msg_open_server(bool create);
MsgQueue_t* msg_create_client();
MsgQueue_t* msg_queue_from_id(const char* id_str);
void msg_close(MsgQueue_t* msg_queue);
void msg_destroy(MsgQueue_t* msg_queue);
void msg_free(MsgQueue_t* msg_queue);

/**
 * Waits until a message of type 'type' appears.
 * If a message of a different type appears (one that the server can send unprompted), the 'pending_handler' will
 * be called, and the waiting will be resumed.
 */
int msg_wait_for_type(MsgQueue_t* queue, Data_t* data, MsgType_t type, MessageHandler_t pending_handler);

/**
 * Fetches pending messages that the server can send unprompted, and calls the 'handler' for each of them.
 * This function DOES NOT wait for anything.
 */
int msg_fetch_pending(MsgQueue_t* queue, Data_t *data, MessageHandler_t handler);

/**
 * Fetches all pending messages, and calls the 'handler' for each of them.
 * This function DOES NOT wait for anything.
 */
int msg_fetch_all(MsgQueue_t* queue, Data_t *data, MessageHandler_t handler);
void msg_send_str(MsgQueue_t* queue, MsgType_t msg_type, const char* msg_content);
int msg_get_queue_id(MsgQueue_t* queue, char* buffer);

const char* msg_get_error();

#endif //ZAD1_QUEUE_H