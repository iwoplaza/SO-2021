#include "msgqueue.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void mq_init(MessageQueue_t* queue)
{
    queue->head = NULL;
    queue->tail = NULL;
}

void mq_push_back(MessageQueue_t* queue, const char* contents)
{
    Message_t* element = malloc(sizeof(Message_t));

    element->next = NULL;
    strcpy(element->contents, contents);

    if (queue->tail == NULL)
    {
        queue->head = element;
        queue->tail = element;
    }
    else
    {
        queue->tail->next = element;
    }
}

void mq_pop_front(MessageQueue_t* queue, Message_t** msg_out)
{
    if (queue->head == NULL)
    {
        fprintf(stderr, "Tried to pop an empty message queue.");
        exit(1);
    }
    
    *msg_out = queue->head;

    // Advancing the head.
    queue->head = queue->head->next;

    if (queue->head == NULL)
    {
        queue->tail = NULL;
    }
}

bool mq_is_empty(MessageQueue_t* queue)
{
    return queue->head == NULL;
}

static void _free_node(Message_t* message)
{
    if (message != NULL)
    {
        _free_node(message->next);

        free(message);
    }
}

void mq_free(MessageQueue_t* queue)
{
    _free_node(queue->head);
    queue->head = NULL;
}

void mq_print(const MessageQueue_t* queue, const char* label)
{
    printf("%s: ", label);

    Message_t* curr = queue->head;
    while (curr != NULL)
    {
        printf("%s ->", curr->contents);
        curr = curr->next;
    }

    printf(" *\n");
}