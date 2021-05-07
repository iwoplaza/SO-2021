#include "queue.h"
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

char msg_error[256];

MsgQueue_t* msg_open_server(bool create)
{
    key_t key = ftok(QUEUE_FILEPATH, 'S');

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

    MsgQueue_t* queue = malloc(sizeof(MsgQueue_t));
    queue->id = id;

    return queue;
}

MsgQueue_t* msg_create_client()
{
    // Trying to create a client queue.
    int id = msgget(IPC_PRIVATE, S_IRWXU | S_IRWXG | S_IRWXO);

    MsgQueue_t* queue = malloc(sizeof(MsgQueue_t));
    queue->id = id;

    return queue;
}

MsgQueue_t* msg_queue_from_id(const char* id_str)
{
    int id = atoi(id_str);

    MsgQueue_t* queue = malloc(sizeof(MsgQueue_t));
    queue->id = id;

    return queue;
}

void msg_close(MsgQueue_t* msg_queue)
{
    // There is no such thing as "closing" a connection in System V.
}

void msg_destroy(MsgQueue_t* msg_queue)
{
    // Destroying the queue.
    msgctl(msg_queue->id, IPC_RMID, NULL);

    // Freeing the container's memory.
    free(msg_queue);
}

int msg_receive(MsgQueue_t* queue, Data_t* data)
{
    return msgrcv(queue->id, data, MAX_MSG_LENGTH, 0, 0);
}

void msg_send_str(MsgQueue_t* queue, MsgType_t msg_type, char* msg_content)
{
    Data_t data;

    data.type = msg_type;
    strcpy(data.buffer, msg_content);

    msgsnd(queue->id, &data, MAX_MSG_LENGTH, 0);
}

int msg_get_queue_id(MsgQueue_t* queue, char* buffer)
{
    return sprintf(buffer, "%d", queue->id);
}

const char* msg_get_error()
{
    return msg_error;
}