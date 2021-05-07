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
    int client_id = atoi(request_data->buffer);

    printf("We listin for %d\n", client_id);
    // Sending back the Client ID assigned to the newly connected client.
    char* offset_buffer = response_buffer;

    offset_buffer += sprintf(offset_buffer, "%d ", clients_amount);

    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        if (clients[i].queue != NULL)
        {
            offset_buffer += sprintf(offset_buffer, "%d %d ", i, clients[i].connected_to);
        }
    }

    msg_send_str(clients[client_id].queue, MSG_LIST, response_buffer);
}

void handle_pending(Data_t* data)
{
    switch (data->type)
    {
        case MSG_INIT:
            handle_init(data);
            break;
        case MSG_STOP:
            running = false;
            break;
        case MSG_LIST:
            handle_list_command(data);
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

    // Closing the server queue.
    msg_close(server_queue);

    // Destroying the server queue.
    msg_destroy(server_queue);
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
