#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "queue.h"
#include "utils.h"

MsgQueue_t* server_queue;
MsgQueue_t* client_queue;
int client_id = -1;
char client_queue_id[64];

bool running = true;

void send_init()
{
    msg_send_str(server_queue, MSG_INIT, client_queue_id);

    // Waiting for response
    Data_t response_data;
    msg_receive(client_queue, &response_data);

    if (response_data.type != MSG_INIT)
    {
        fprintf(stderr, "Expected a message of type 'INIT', got %ld ('%s') instead.\n", response_data.type, MSG_TYPE_NAME[response_data.type]);
        exit(1);
    }

    // Parsing the client id.
    client_id = atoi(response_data.buffer);
}

void handle_shutdown(int signo)
{
    printf("Shutting down...\n");

    // Destroying the client queue
    msg_close(server_queue);
    msg_destroy(client_queue);

    exit(0);
}

void init()
{
    printf(INIT_LOG_FMT, "Setting up client queue... ");
    client_queue = msg_create_client();
    if (client_queue == NULL)
    {
        print_error();
        fprintf(stderr, "\033[31m%s\n", msg_get_error());
        exit(1);
    }

    msg_get_queue_id(client_queue, client_queue_id);
    print_ok_msg("Queue ID: %s", client_queue_id);

    // -------

    printf(INIT_LOG_FMT, "Connecting to server queue... ");
    server_queue = msg_open_server(false);

    if (server_queue == NULL)
    {
        print_error();
        fprintf(stderr, "\033[31m%s\n", msg_get_error());
        exit(1);
    }

    print_ok();

    // -------

    printf(INIT_LOG_FMT, "Initializing... ");
    send_init();
    print_ok_msg("Client id: %d", client_id);
}

void handle_commands()
{
    char cmd_buffer[256];
    
    while (running)
    {

        Data_t request_data;
        msg_receive(server_queue, &request_data);

        switch (request_data.type)
        {
            case MSG_STOP:
                running = false;
                break;
        }
    }
}

int main()
{
    signal(SIGINT, handle_shutdown);

    printf("============\n");
    printf("== CLIENT ==\n");
    printf("============\n\n");

    init();

    // Flushing before setting up the listener loop.
    fflush(stdout);

    handle_commands();

    handle_shutdown(0);

    return 0;
}
