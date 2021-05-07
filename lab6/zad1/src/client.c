#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "queue.h"
#include "utils.h"

MsgQueue_t* server_queue;
MsgQueue_t* client_queue;
char client_queue_id[64];
int client_id = -1;
char client_id_str[64];

bool running = true;
MsgQueue_t* connected_to_queue;

const char* HELP_STR = \
    COL_YELLOW "LIST        " COL_RESET " - Show all clients connected to the server.\n" \
    COL_YELLOW "CONNECT <id>" COL_RESET " - Connects the client with another client of id <id>, if they're available.\n" \
    COL_YELLOW "STOP        " COL_RESET " - Closes the client.\n";

void handle_pending(Data_t* data)
{
    printf("Received pending message (%s): %s\n", MSG_TYPE_NAME[data->type], data->buffer);

    if (data->type == MSG_STOP)
    {
        running = false;
    }
    else if (data->type == MSG_DISCONNECT)
    {
        if (connected_to_queue != NULL)
        {
            msg_close(connected_to_queue);
        }

        connected_to_queue = NULL;
    }
    else if (data->type == MSG_CONNECT)
    {
        if (connected_to_queue != NULL)
        {
            // Already connected.
            return;
        }

        connected_to_queue = msg_queue_from_id(data->buffer);
    }
    else
    {
        printf("Received an invalid pending message (%s): %s\n", MSG_TYPE_NAME[data->type], data->buffer);
    }
}

void send_init()
{
    msg_send_str(server_queue, MSG_INIT, client_queue_id);

    // Waiting for response
    Data_t response_data;
    msg_wait_for_type(client_queue, &response_data, MSG_INIT, handle_pending);

    // Parsing the client id.
    client_id = atoi(response_data.buffer);
    strcpy(client_id_str, response_data.buffer);
}

void handle_shutdown()
{
    printf("Shutting down...\n");

    // Destroying the client queue
    msg_close(server_queue);
    msg_destroy(client_queue);

    // Disconnecting from potential client
    if (connected_to_queue != NULL)
    {
        msg_close(connected_to_queue);
    }

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

void handle_list_command()
{
    msg_send_str(server_queue, MSG_LIST, client_id_str);
    // Waiting for response
    Data_t response_data;
    msg_wait_for_type(client_queue, &response_data, MSG_LIST, handle_pending);

    // Preparing for parsing the response.
    char* offset_buffer = response_data.buffer;

    int amount_of_clients = -1;
    offset_buffer += sscanint(offset_buffer, &amount_of_clients) + 1;

    printf("List:\n");
    for (int i = 0; i < amount_of_clients; ++i)
    {
        int id;
        int connected_to;

        offset_buffer += sscanint(offset_buffer, &id) + 1;
        offset_buffer += sscanint(offset_buffer, &connected_to) + 1;

        printf("- %d ", id);
        if (id == client_id)
        {
            printf("[me]");
        }
        else
        {
            if (connected_to != -1)
            {
                printf(COL_YELLOW "[Talking to %d]", connected_to);
            }
            else
            {
                printf(COL_BLUE "[Available]");
            }
        }
        printf("\n" COL_RESET);
    }

    printf("\n");
}

void handle_commands()
{
    char cmd_buffer[256];
    
    while (running)
    {
        printf("> ");
        scanf("%s", cmd_buffer);

        if (strcmp(cmd_buffer, "LIST") == 0)
        {
            handle_list_command();
        }
        else if(strcmp(cmd_buffer, "HELP") == 0)
        {
            printf("%s", HELP_STR);
        }
        else if(strcmp(cmd_buffer, "STOP") == 0)
        {
            running = false;
        }
        else
        {
            printf("Unkown command. Type 'HELP' for list of commands.\n");
        }
    }
}

void handle_interrupt()
{
    exit(0);
}

int main()
{
    signal(SIGINT, handle_interrupt);
    atexit(handle_shutdown);

    printf("============\n");
    printf("== CLIENT ==\n");
    printf("============\n\n");

    init();

    // Flushing before setting up the listener loop.
    fflush(stdout);

    handle_commands();

    return 0;
}
