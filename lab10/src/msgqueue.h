#ifndef LAB10_MSGQUEUE_H
#define LAB10_MSGQUEUE_H

#include <stdbool.h>

#define MAX_MSG_LEN 64

typedef struct Message_t {
    char contents[MAX_MSG_LEN];
    struct Message_t* next;
} Message_t;

typedef struct MessageQueue_t {
    Message_t* head;
    Message_t* tail;
} MessageQueue_t;

void mq_init(MessageQueue_t* queue);
void mq_push_back(MessageQueue_t* queue, const char* contents);
void mq_pop_front(MessageQueue_t* queue, Message_t** msg_out);
bool mq_is_empty(MessageQueue_t* queue);
void mq_free(MessageQueue_t* queue);
void mq_print(const MessageQueue_t* queue, const char* label);

#endif //LAB10_MSGQUEUE_H