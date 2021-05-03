#ifndef ZAD1_QUEUE_H
#define ZAD1_QUEUE_H

#include <stdbool.h>
#include "defs.h"

#define MAX_MSG_LENGTH 64

typedef struct Data_t {
    long type;
    char buffer[MAX_MSG_LENGTH];
} Data_t;

typedef struct MsgQueue_t {
    int id;
} MsgQueue_t;

MsgQueue_t* msg_open_server(bool create);
MsgQueue_t* msg_create_client();
MsgQueue_t* msg_queue_from_id(const char* id_str);
void msg_close(MsgQueue_t* msg_queue);
void msg_destroy(MsgQueue_t* msg_queue);
int msg_receive(MsgQueue_t* queue, Data_t* data);
void msg_send_str(MsgQueue_t* queue, MsgType_t msg_type, char* msg_content);
int msg_get_queue_id(MsgQueue_t* queue, char* buffer);

const char* msg_get_error();

#endif //ZAD1_QUEUE_H