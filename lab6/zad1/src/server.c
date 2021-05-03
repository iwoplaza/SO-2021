#include <stdio.h>
#include <string.h>
#include "queue.h"

#define MAX_CLIENTS 16

typedef struct Client_t {
    MsgQueue_t* queue;
} Client_t;

MsgQueue_t* server_queue;
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

        printf("Got message of type %ld", request_data.type);
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
}

int main()
{
    server_queue = msg_open_server(true);
    
    if (server_queue == NULL)
    {
        return 1;
    }

    printf("Server has been set up.\n");

    handle_requests();

    handle_shutdown();

    return 0;
}
