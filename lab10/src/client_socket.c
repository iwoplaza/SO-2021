#include "client_socket.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "defs.h"
#include "logs.h"

#define MSG_BUFFER_LEN 256


static char msg_buffer[MSG_BUFFER_LEN];
static MessageQueue_t msg_queue;

static void handle_ping_msg(char* msg, int socket_fd)
{
    sprintf(msg_buffer, "%d", MSG_PING);
    if (!comm_send_msg(socket_fd, msg_buffer))
    {
        fprintf(stderr, "Failed to send back PING message.\n");
        return;
    }
}

static void handle_move_msg(char* msg_buffer, GameState_t* state, int client_fd)
{
    // Reading message components.
    int dummy_msg_type;
    char serialized_game_board[STATE_SERIALIZATION_LEN];

    sscanf(msg_buffer, "%d %s", &dummy_msg_type, serialized_game_board);

    // Using the server serialized game state to update our own local state.
    deserialize_game_state(serialized_game_board, state);
}

static void* socket_thread_routine(void* args)
{
    SocketThreadArgs_t* thread_args = args;
    ClientComm_t* client = thread_args->client;

    printf("SOCKET THREAD START\n");

    while (thread_args->running)
    {
        if (!comm_read_msgs(client->socket_fd, &msg_queue))
        {
            fprintf(stderr, "Failed to read message in message loop.\n");
            exit(1);
        }

        MessageType_t msg_type;
        sscanf(msg_buffer, "%d", &msg_type);

        if (msg_type < 0 || msg_type >= MSG_MAX)
        {
            printf("Unknown message type: %d\n", msg_type);
            continue;
        }

        printf("Received a %s message: %s\n", MESSAGE_TYPE_NAMES[msg_type], msg_buffer);

        switch (msg_type)
        {
            case MSG_PING:
                handle_ping_msg(msg_buffer, client->socket_fd);
                break;
            case MSG_JOIN:
                // Ignore, shouldn't receive this.
                fprintf(stderr, "WARNING: Got a join message from server after the joining phase.\n");
                break;
            case MSG_MOVE:
                handle_move_msg(msg_buffer, thread_args->game_state, client->socket_fd);
                break;
            case MSG_QUIT:
                thread_args->running = false;
                break;
            default:
                fprintf(stderr, "Received message of unknown type: %d\n", msg_type);
                break;
        }
    }

    printf("SOCKET THREAD END\n");

    // Freeing the memory after use.
    free(thread_args);

    return NULL;
}

static pthread_t create_socket_thread(SocketThreadArgs_t* args)
{
    pthread_t thread;

    if (pthread_create(&thread, NULL, socket_thread_routine, args) != 0)
    {
        perror("Failed to create thread.");
        return -1;
    }

    return thread;
}

int handle_join_msg(const char* username)
{
    Message_t* msg;
    mq_pop_front(&msg_queue, &msg);

    expect_msg_type(msg->contents, MSG_JOIN);

    int dummy_type;
    JoinStatus_t status;

    sscanf(msg->contents, "%d %d", &dummy_type, &status);

    if (status == JOIN_SUCCESS)
    {
        return 0;
    }
    else if (status == JOIN_ALREADY_FULL)
    {
        printf(COL_YELLOW "The server is already full. Try another time.\n" COL_RESET);
        return -1;
    }
    else if (status == JOIN_NAME_TAKEN)
    {
        printf(COL_YELLOW "The username \"%s\" is taken.\n" COL_RESET, username);
        return -1;
    }

    fprintf(stderr, "Malformed packet, unknown join status: %d\n", status);
    exit(1);
}

void handle_start_msg(SocketThreadArgs_t* args)
{
    Message_t* msg;
    mq_pop_front(&msg_queue, &msg);

    expect_msg_type(msg->contents, MSG_START);

    int dummy_type;
    char serialized_state[STATE_SERIALIZATION_LEN];

    sscanf(msg->contents, "%d %s", &dummy_type, serialized_state);

    deserialize_game_state(serialized_state, args->game_state);
}

pthread_t init_sockets(SocketThreadArgs_t* args, const char* username)
{
    mq_init(&msg_queue);

    // Sending the join request
    sprintf(msg_buffer, "%d %s", MSG_JOIN, username);
    if (!comm_send_msg(args->client->socket_fd, msg_buffer))
    {
        fprintf(stderr, "Failed to send JOIN request\n");
        return -1;
    }

    // DEBUG Printing the queue
    mq_print(&msg_queue, "before read join");

    if (!comm_read_msgs(args->client->socket_fd, &msg_queue))
    {
        fprintf(stderr, "Failed to read response to JOIN request\n");
        return -1;
    }

    // DEBUG Printing the queue
    mq_print(&msg_queue, "after read join");

    if (mq_is_empty(&msg_queue))
    {
        fprintf(stderr, "The JOIN message is not on the queue.\n");
        return -1;
    }

    if (handle_join_msg(username) == -1)
    {
        return -1;
    }

    // DEBUG Printing the queue
    mq_print(&msg_queue, "after handle join");

    printf("Joined room, waiting for the game to start...\n");

    if (mq_is_empty(&msg_queue))
    {
        // Fetching possible new start message.
        if (!comm_read_msgs(args->client->socket_fd, &msg_queue))
        {
            fprintf(stderr, "Failed to read START request\n");
            return -1;
        }
    }

    // DEBUG Printing the queue
    mq_print(&msg_queue, "after filling of start");

    if (mq_is_empty(&msg_queue))
    {
        fprintf(stderr, "The START message is not on the queue.\n");
        return -1;
    }

    // DEBUG Printing the queue
    mq_print(&msg_queue, "after empty check");

    handle_start_msg(args);

    // DEBUG Printing the queue
    mq_print(&msg_queue, "after start handle");

    pthread_t thread_id = create_socket_thread(args);
    return thread_id;
}
