#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "queue.h"
#include "utils.h"

MsgQueue_t* server_queue;
MsgQueue_t* client_queue;
int client_id = -1;
char client_queue_id[64];

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
}

int main()
{
    signal(SIGINT, handle_shutdown);

    printf("============\n");
    printf("== CLIENT ==\n");
    printf("============\n\n");

    // -------

    printf("Setting up client queue... ");
    client_queue = msg_create_client();
    msg_get_queue_id(client_queue, client_queue_id);

    if (client_queue == NULL)
    {
        print_error();
        fprintf(stderr, "\033[31m%s\n", msg_get_error());
        return 1;
    }
    print_ok();

    // -------

    printf("Connecting to server queue... ");
    server_queue = msg_open_server(false);

    if (server_queue == NULL)
    {
        print_error();
        fprintf(stderr, "\033[31m%s\n", msg_get_error());
        return 1;
    }

    print_ok();

    // -------

    printf("Initializing... ");
    send_init();
    print_ok_msg(" - received client id: %d", client_id);

    handle_shutdown(0);

    return 0;
}
