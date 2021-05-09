#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "queue.h"
#include "utils.h"

#define MAX_CLIENTS 10

typedef struct Client_t {
    MsgQueue_t* queue; // NULL means this client slot is empty.
    int connected_to; // -1 means this client's not connected to anyone.
} Client_t;

MsgQueue_t* server_queue;
char server_queue_id[64];
Client_t clients[MAX_CLIENTS];
int clients_amount = 0;
bool running = true;

// A buffer used to construct responses.
char response_buffer[MAX_MSG_LENGTH];

void handle_init(const Data_t* request_data)
{
    MsgQueue_t* queue = msg_queue_from_id(request_data->buffer);

    // Trying to find a free client ID.
    int client_id = 0;
    while (client_id < MAX_CLIENTS && clients[client_id].queue != NULL)
        client_id++;

    if (client_id >= MAX_CLIENTS)
    {
        // Sending back -1 to tell the client that we couldn't initialize them.
        sprintf(response_buffer, "%d", -1);
        msg_send_str(queue, MSG_INIT, response_buffer);
        return;
    }

    printf("Client %d has connected.\n", client_id);

    // Creating a queue instance from the id given by the client.
    clients[client_id].queue = queue;

    // Sending back the Client ID assigned to the newly connected client.
    sprintf(response_buffer, "%d", client_id);
    msg_send_str(queue, MSG_INIT, response_buffer);

    clients_amount++;
}

void handle_list_command(const Data_t* request_data)
{
    // Fetching parameters from the request.
    int client_id = atoi(request_data->buffer);

    // Checking for id validity
    if (client_id < 0 || client_id >= MAX_CLIENTS)
    {
        fprintf(stderr, "Received a corrupted LIST message. %d tried to list.\n", client_id);
        return;
    }

    printf("Client %d has requested a LIST.\n", client_id);

    // Preparing the response.

    char* offset_buffer = response_buffer;

    offset_buffer += sprintf(offset_buffer, "%d ", clients_amount);

    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (clients[i].queue != NULL)
        {
            offset_buffer += sprintf(offset_buffer, "%d %d ", i, clients[i].connected_to);
        }
    }

    // Sending the response
    msg_send_str(clients[client_id].queue, MSG_LIST, response_buffer);
}

void handle_connect_command(const Data_t* request_data)
{
    int client_id;
    int connect_to;

    // Fetching parameters from the request.
    sscanf(request_data->buffer, "%d %d", &client_id, &connect_to);

    // Checking for id validity
    if (client_id < 0 || client_id >= MAX_CLIENTS || connect_to < 0 || connect_to >= MAX_CLIENTS)
    {
        fprintf(stderr, "Received a corrupted CONNECT message. %d tried to connect with %d.\n", client_id, connect_to);

        // Sending the empty string as a response to the connection.
        msg_send_str(clients[client_id].queue, MSG_CONNECT, "");
        return;
    }

    // Checking if perhaps the client isn't connecting to itself.
    if (client_id == connect_to)
    {
        printf(COL_YELLOW "Client %d tried to connect with itself.\n" COL_RESET, client_id);
        // Sending the empty string as a response to the connection.
        msg_send_str(clients[client_id].queue, MSG_CONNECT, "");
        return;
    }

    // Checking if the requesting client is free to talk.
    if (clients[client_id].connected_to != -1)
    {
        // The requesting user is already connected to someone.
        //
        // This can happen when a user tried to connect and is
        // simultaneously being connected to.
        //
        // Sending the empty string as a response to the connection.
        msg_send_str(clients[client_id].queue, MSG_CONNECT, "");
        return;
    }

    // Checking if the target client is free to talk.
    if (clients[connect_to].queue == NULL || clients[connect_to].connected_to != -1)
    {
        // The target user either doesn't exist or is already connected to someone.
        // Sending the empty string as a response to the connection.
        msg_send_str(clients[client_id].queue, MSG_CONNECT, "");
        return;
    }

    // Updating the server state
    clients[client_id].connected_to = connect_to;
    clients[connect_to].connected_to = client_id;

    printf(COL_CYAN "Client %d" COL_RESET " connected with " COL_CYAN "Client %d " COL_RESET ".\n", client_id, connect_to);

    // Sending both clients the CONNECT command with each other's queue ids.
    // One treats it like a response, the other treats it like a pending message.
    msg_get_queue_id(clients[connect_to].queue, response_buffer);
    msg_send_str(clients[client_id].queue, MSG_CONNECT, response_buffer);
    msg_get_queue_id(clients[client_id].queue, response_buffer);
    msg_send_str(clients[connect_to].queue, MSG_CONNECT, response_buffer);
}

void handle_disconnect_command(const Data_t* request_data)
{
    // Fetching parameters from the request.
    int client_id = atoi(request_data->buffer);

    // Checking for id validity
    if (client_id < 0 || client_id >= MAX_CLIENTS)
    {
        fprintf(stderr, "Received a corrupted DISCONNECT message. %d tried to disconnect.\n", client_id);
        return;
    }

    // Checking if the client is currently connected to someone
    if (clients[client_id].connected_to != -1)
    {
        // Sending a disconnect message to the target client, so that both clients disconnect from each other.
        msg_send_str(clients[clients[client_id].connected_to].queue, MSG_DISCONNECT, "");

        printf(COL_CYAN "Client %d" COL_RESET " disconnected with " COL_CYAN "Client %d " COL_RESET ".\n",
               client_id, clients[client_id].connected_to);

        clients[clients[client_id].connected_to].connected_to = -1;
        clients[client_id].connected_to = -1;
    }
}

/**
 * This command gets sent by a client rught before it finishes execution.
 * @param request_data The data sent is just the `client_id`.
 */
void handle_stop_command(const Data_t* request_data)
{
    // Fetching parameters from the request.
    int client_id = atoi(request_data->buffer);

    // Checking for id validity
    if (client_id < 0 || client_id >= MAX_CLIENTS)
    {
        fprintf(stderr, "Received a corrupted STOP message. %d tried to inform about it's shutdown.\n", client_id);
        return;
    }

    // Checking if the client is currently connected to someone
    if (clients[client_id].connected_to != -1)
    {
        // Sending a disconnect message to the target client, so that both clients disconnect from each other.
        msg_send_str(clients[clients[client_id].connected_to].queue, MSG_DISCONNECT, "");

        // Disconnecting them from us.
        clients[clients[client_id].connected_to].connected_to = -1;
    }
    
    // Closing the client's queue
    msg_close(clients[client_id].queue);

    // Deinitializing the client
    msg_free(clients[client_id].queue);
    clients[client_id].queue = NULL;
    clients[client_id].connected_to = -1;

    // Decreasing the amount of clients.
    clients_amount--;

    printf("Client %d has disconnected.\n", client_id);
}

void handle_pending(Data_t* data)
{
    switch (data->type)
    {
        case MSG_INIT:
            handle_init(data);
            break;
        case MSG_LIST:
            handle_list_command(data);
            break;
        case MSG_CONNECT:
            handle_connect_command(data);
            break;
        case MSG_DISCONNECT:
            handle_disconnect_command(data);
            break;
        case MSG_STOP:
            handle_stop_command(data);
            break;
    }
}

void handle_requests()
{
    while (running)
    {
        Data_t request_data;
        msg_fetch_all(server_queue, &request_data, handle_pending);

        usleep(100);
    }
}

void handle_shutdown()
{
    puts("Stopping...\n");

    if (server_queue != NULL)
    {
        // Destroying the server queue.
        msg_destroy(server_queue);

        // Freeing memory.
        msg_free(server_queue);
    }

    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (clients[i].queue != NULL)
        {
            // Sending a stop message to the target client, informing about the server's shutdown.
            msg_send_str(clients[i].queue, MSG_STOP, "");

            // Closing the client queue.
            msg_close(clients[i].queue);

            // Freeing memory.
            msg_free(clients[i].queue);
            clients[i].queue = NULL;
        }
    }
}

void handle_interrupt()
{
    exit(0);
}

void init()
{
    // Initializing the client array
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        clients[i].queue = NULL;
        clients[i].connected_to = -1;
    }

    printf(INIT_LOG_FMT, "Setting up server queue... ");
    server_queue = msg_open_server(true);
    if (server_queue == NULL)
    {
        print_error();
        fprintf(stderr, "\033[31m%s\n", msg_get_error());
        exit(1);
    }

    msg_get_queue_id(server_queue, server_queue_id);
    print_ok_msg("Queue ID: %s", server_queue_id);
}

int main()
{
    signal(SIGINT, handle_interrupt);
    atexit(handle_shutdown);

    printf("============\n");
    printf("== SERVER ==\n");
    printf("============\n\n");

    init();

    printf("Server has been set up.\n");
    // Flushing before setting up the listener loop.
    fflush(stdout);

    handle_requests();

    handle_shutdown();

    return 0;
}
