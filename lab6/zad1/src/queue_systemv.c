#include "queue.h"
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

char msg_error[256];

MsgQueue_t *msg_open_server(bool create)
{
    key_t key = ftok(getenv("HOME"), QUEUE_SERVER_SYMBOL);

    // Trying to create/open a server message queue.
    int id = msgget(key, (create ? IPC_CREAT : 0) | S_IRWXU | S_IRWXG | S_IRWXO);

    if (id == -1)
    {
        if (errno == ENOENT)
        {
            sprintf(msg_error, "Failed to open server queue. It doesn't exist.");
        }
        else
        {
            sprintf(msg_error, "Failed to open server queue. Errno: %d.", errno);
        }
        return NULL;
    }

    MsgQueue_t *queue = malloc(sizeof(MsgQueue_t));
    queue->id = id;

    return queue;
}

MsgQueue_t *msg_create_client()
{
    // Trying to create a client queue.
    int id = msgget(IPC_PRIVATE, S_IRWXU | S_IRWXG | S_IRWXO);

    MsgQueue_t *queue = malloc(sizeof(MsgQueue_t));
    queue->id = id;

    return queue;
}

MsgQueue_t *msg_queue_from_id(const char *id_str)
{
    int id = atoi(id_str);

    MsgQueue_t *queue = malloc(sizeof(MsgQueue_t));
    queue->id = id;

    return queue;
}

void msg_close(MsgQueue_t *msg_queue)
{
    // There is no such thing as "closing" a connection in System V.
}

void msg_destroy(MsgQueue_t *msg_queue)
{
    // Destroying the queue.
    msgctl(msg_queue->id, IPC_RMID, NULL);
}

void msg_free(MsgQueue_t *msg_queue)
{
    // Freeing the container's memory.
    free(msg_queue);
}

int _fetch_pending_of_type(MsgQueue_t *queue, Data_t *data, MsgType_t type, MessageHandler_t pending_handler)
{
    // Checking if we perhaps received a 'type' message.
    int status = msgrcv(queue->id, data, MAX_MSG_LENGTH, type, IPC_NOWAIT);

    if (status != -1)
    {
        // We had a 'type' message pending.
        pending_handler(data);
    }
    else if (errno != ENOMSG)
    {
        sprintf(msg_error, "Couldn't read from the queue, Errno: %d", errno);
        return -1;
    }

    return 0;
}

int msg_wait_for_type(MsgQueue_t *queue, Data_t *data, MsgType_t type, MessageHandler_t pending_handler)
{
    if (msg_fetch_pending(queue, data, pending_handler) == -1)
        return -1;

    if (msgrcv(queue->id, data, MAX_MSG_LENGTH, type, 0) == -1)
    {
        sprintf(msg_error, "Couldn't read from the queue, Errno: %d", errno);
        return -1;
    }

    return 0;
}

int msg_fetch_pending(MsgQueue_t* queue, Data_t *data, MessageHandler_t handler)
{
    // Checking if we perhaps received any of the following messages.

    if (_fetch_pending_of_type(queue, data, MSG_STOP, handler) == -1)
        return -1;

    if (_fetch_pending_of_type(queue, data, MSG_DISCONNECT, handler) == -1)
        return -1;

    if (_fetch_pending_of_type(queue, data, MSG_CONNECT, handler) == -1)
        return -1;

    if (_fetch_pending_of_type(queue, data, MSG_CHAT, handler) == -1)
        return -1;

    return 0;
}

int msg_fetch_all(MsgQueue_t* queue, Data_t *data, MessageHandler_t handler)
{
    // Fetching pending message from last to first.
    for (int i = MSG_TYPES - 1; i >= 1; --i)
    {
        if (_fetch_pending_of_type(queue, data, i, handler) == -1)
            return -1;
    }

    return 0;
}

void msg_send_str(MsgQueue_t *queue, MsgType_t msg_type, const char *msg_content)
{
    Data_t data;

    data.type = msg_type;
    strcpy(data.buffer, msg_content);

    msgsnd(queue->id, &data, MAX_MSG_LENGTH, 0);
}

int msg_get_queue_id(MsgQueue_t *queue, char *buffer)
{
    return sprintf(buffer, "%d", queue->id);
}

const char *msg_get_error()
{
    return msg_error;
}