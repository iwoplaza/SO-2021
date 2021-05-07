#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include "queue.h"
#include "utils.h"

#define MAX_CLIENTS 16

typedef struct Client_t {
    MsgQueue_t* queue;
} Client_t;

MsgQueue_t* server_queue;
char server_queue_id[64];
Client_t clients[MAX_CLIENTS];
int clients_amount = 0;
bool running = true;

// A buffer used to construct responses.
char response_buffer[256];


void handle_init(const Data_t* request_data)
{
    int client_id = clients_amount;
    printf("Client %d has connected.\n", client_id);

    // Creating a queue instance from the id given by the client.
    clients[client_id].queue = msg_queue_from_id(request_data->buffer);

    // Sending back the Client ID assigned to the newly connected client.
    sprintf(response_buffer, "%d", client_id);
    msg_send_str(clients[client_id].queue, MSG_INIT, response_buffer);

    clients_amount++;
}

void handle_requests()
{
    while (running)
    {
        Data_t request_data;
        msg_receive(server_queue, &request_data);

        switch (request_data.type)
        {
            case MSG_INIT:
                handle_init(&request_data);
                break;
            case MSG_STOP:
                running = false;
                break;
        }
    }
}

void handle_shutdown()
{
    printf("Stopping...\n");

    // Closing the server queue.
    msg_close(server_queue);

    // Destroying the server queue.
    msg_destroy(server_queue);

    exit(0);
}

void init()
{
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
    signal(SIGINT, handle_shutdown);

    printf("============\n");
    printf("== SERVER ==\n");
    printf("============\n\n");

    init();

    printf("Server has been set up.");
    // Flushing before setting up the listener loop.
    fflush(stdout);

    handle_requests();

    handle_shutdown();

    return 0;
}
