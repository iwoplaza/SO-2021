#include "queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <mqueue.h>
#include <limits.h>

static char msg_buffer[MAX_MSG_LENGTH + 1];
char msg_error[256];

MsgQueue_t *msg_open_server(bool create)
{
    // Allocating the filepath, so that the freeing later has something to free.
    // The memory is freed anyways because the client filepaths are dynamic.
    char* filepath = malloc(strlen("/lab6-server") + 1);
    strcpy(filepath, "/lab6-server");

    struct mq_attr queue_attr;
    queue_attr.mq_flags = 0;
    queue_attr.mq_maxmsg = 10;
    queue_attr.mq_msgsize = MAX_MSG_LENGTH + 1;
    queue_attr.mq_curmsgs = 0;
    mqd_t id = mq_open(filepath, create ? (O_CREAT | O_RDONLY) : O_WRONLY, S_IRUSR | S_IWUSR, &queue_attr);

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
    queue->filepath = filepath;

    return queue;
}

MsgQueue_t *msg_create_client()
{
    static char filepath_buffer[256];
    int id;
    mqd_t queue_id;

    struct mq_attr queue_attr;
    queue_attr.mq_flags = 0;
    queue_attr.mq_maxmsg = 10;
    queue_attr.mq_msgsize = MAX_MSG_LENGTH + 1;
    queue_attr.mq_curmsgs = 0;

    while (true)
    {
        // Generating a random ID.
        id = rand();
        sprintf(filepath_buffer, "/lab6-client-%d", id);
        // Trying to create a client queue.
        queue_id = mq_open(filepath_buffer, O_RDONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, &queue_attr);

        if (queue_id == -1)
        {
            if (errno == EEXIST)
            {
                // Retry.
                continue;
            }
            else
            {
                sprintf(msg_error, "Failed to create client queue. Errno: %d.", errno);
                return NULL;
            }
        }

        // We have a proper id.
        break;
    }

    char* filepath = malloc(strlen(filepath_buffer) + 1);
    strcpy(filepath, filepath_buffer);

    MsgQueue_t *queue = malloc(sizeof(MsgQueue_t));
    queue->id = queue_id;
    queue->filepath = filepath;

    return queue;
}

MsgQueue_t *msg_queue_from_id(const char *id_str)
{
    MsgQueue_t *queue = malloc(sizeof(MsgQueue_t));
    queue->filepath = malloc(strlen(id_str) + 1);
    strcpy(queue->filepath, id_str);

    queue->id = mq_open(queue->filepath, O_RDWR); // We don't know which way the queue goes, so we choose bi-directional.

    if (queue->id == -1)
    {
        sprintf(msg_error, "Failed to open queue from filepath: %s. Errno: %d.", id_str, errno);
    }

    return queue;
}

int msg_get_queue_id(MsgQueue_t *queue, char *buffer)
{
    return sprintf(buffer, "%s", queue->filepath);
}

void msg_close(MsgQueue_t *msg_queue)
{
    mq_close(msg_queue->id);
}

void msg_destroy(MsgQueue_t *msg_queue)
{
    // Closing it first.
    msg_close(msg_queue);

    // Destroying the queue.
    mq_unlink(msg_queue->filepath);
}

void msg_free(MsgQueue_t *msg_queue)
{
    // Freeing the container's memory.
    free(msg_queue);
}

int msg_wait_for_type(MsgQueue_t *queue, Data_t *data, MsgType_t type, MessageHandler_t pending_handler)
{
    while (true)
    {
        if (mq_receive(queue->id, msg_buffer, MAX_MSG_LENGTH + 1, NULL) == -1)
        {
            sprintf(msg_error, "Couldn't read from the queue, Errno: %d", errno);
            return -1;
        }

        data->type = (int) msg_buffer[0];
        memcpy(data->buffer, msg_buffer + 1, MAX_MSG_LENGTH);

        if (data->type == type)
        {
            // We got the message we were waiting for.
            break;
        }

        // We had a message in-between the one we were waiting for.
        pending_handler(data);
    }

    return 0;
}

int msg_fetch_pending(MsgQueue_t* queue, Data_t *data, MessageHandler_t handler)
{
    // Checking if we perhaps received any of the following messages.
    return msg_fetch_all(queue, data, handler);
}

int msg_fetch_all(MsgQueue_t* queue, Data_t *data, MessageHandler_t handler)
{
    // Changing the behavior of the queue to NOBLOCK
    struct mq_attr old_attr;
    struct mq_attr new_attr;
    new_attr.mq_flags = O_NONBLOCK;
    mq_setattr(queue->id, &new_attr, &old_attr);

    while (true)
    {
        if (mq_receive(queue->id, msg_buffer, MAX_MSG_LENGTH + 1, NULL) == -1)
        {
            if (errno == EAGAIN)
            {
                // No more messages to read.
                break;
            }
            else
            {
                sprintf(msg_error, "Couldn't read from the queue, Errno: %d", errno);
                return -1;
            }
        }

        data->type = (int) msg_buffer[0];
        memcpy(data->buffer, msg_buffer + 1, MAX_MSG_LENGTH);

        handler(data);
    }

    // Restoring old settings
    mq_setattr(queue->id, &old_attr, NULL);

    return 0;
}

void msg_send_str(MsgQueue_t *queue, MsgType_t msg_type, const char *msg_content)
{
    msg_buffer[0] = (char) msg_type;
    strcpy(msg_buffer + 1, msg_content);

    mq_send(queue->id, msg_buffer, MAX_MSG_LENGTH + 1, msg_type < MQ_PRIO_MAX ? msg_type : MQ_PRIO_MAX);
}

const char *msg_get_error()
{
    return msg_error;
}