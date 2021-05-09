#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include "queue.h"
#include "utils.h"

MsgQueue_t* server_queue;
MsgQueue_t* client_queue;
char client_queue_id[64];
int client_id = -1;
char client_id_str[64];

char request_buffer[MAX_MSG_LENGTH];
bool running = true;
bool conversing = false;
MsgQueue_t* connected_to_queue;

const char* HELP_STR = \
    COL_YELLOW "list        " COL_RESET " - Show all clients connected to the server.\n" \
    COL_YELLOW "connect <id>" COL_RESET " - Connects the client with another client of id <id>, if they're available.\n" \
    COL_YELLOW "stop        " COL_RESET " - Closes the client.\n";

void run_chat_mode();


void handle_pending(Data_t* data)
{
    if (data->type == MSG_STOP)
    {
        running = false;
    }
    else if (data->type == MSG_DISCONNECT)
    {
        conversing = false;
    }
    else if (data->type == MSG_CONNECT)
    {
        if (connected_to_queue != NULL)
        {
            // Already connected.
            return;
        }

        run_chat_mode(data->buffer);
    }
    else if (data->type == MSG_CHAT)
    {
        printf(COL_YELLOW "OTHER: " COL_RESET "%s\n", data->buffer);
    }
    else
    {
        printf("Received an unsupported pending message (%s): %s\n", MSG_TYPE_NAME[data->type], data->buffer);
    }
}

void send_init()
{
    msg_send_str(server_queue, MSG_INIT, client_queue_id);

    // Waiting for response
    Data_t response_data;
    if (msg_wait_for_type(client_queue, &response_data, MSG_INIT, handle_pending) == -1)
    {
        fprintf(stderr, COL_ERROR "%s\n", msg_get_error());
        return;
    }

    // Parsing the client id.
    client_id = atoi(response_data.buffer);
    strcpy(client_id_str, response_data.buffer);
}

void handle_shutdown()
{
    printf("Shutting down...\n");

    // Sending a message to the server, informing of the client's shutdown.
    if (server_queue != NULL)
    {
        msg_send_str(server_queue, MSG_STOP, client_id_str);

        // Closing the server queue
        msg_close(server_queue);
        msg_free(server_queue);
    }

    if (client_queue != NULL)
    {
        // Destroying the client queue
        msg_destroy(client_queue);
        msg_free(client_queue);
    }

    // Disconnecting from potential client
    if (connected_to_queue != NULL)
    {
        msg_close(connected_to_queue);
        msg_free(connected_to_queue);
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
        fprintf(stderr, COL_ERROR "%s\n", msg_get_error());
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
        fprintf(stderr, COL_ERROR "%s\n", msg_get_error());
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
    if (msg_wait_for_type(client_queue, &response_data, MSG_LIST, handle_pending) == -1)
    {
        fprintf(stderr, COL_ERROR "%s\n", msg_get_error());
        return;
    }

    // Preparing for parsing the response.
    char* offset_buffer = response_data.buffer;

    printf("Got message: %s\n", offset_buffer);

    int amount_of_clients = -1;
    offset_buffer += sscanint(offset_buffer, &amount_of_clients) + 1;

    printf("List: \n");
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
                printf(COL_CYAN "[Available]");
            }
        }
        printf("\n" COL_RESET);
    }

    printf("\n");
}

void run_chat_mode(const char* target_client_queue_id)
{
    static Data_t response_data;

    // Creating a queue from the passed in queue id.
    connected_to_queue = msg_queue_from_id(target_client_queue_id);

    printf("Type '!exit' to stop the connection at any time.\n\n");

    // Reading/Writing messages.

    size_t msg_buffer_size = MAX_MSG_LENGTH;
    char* msg_buffer = malloc(sizeof(char) * msg_buffer_size);
    conversing = true;

    while (conversing)
    {
        int bytes_read = getline(&msg_buffer, &msg_buffer_size, stdin);
        // Removing the newline character.
        msg_buffer[bytes_read - 1] = '\0';

        if (bytes_read >= MAX_MSG_LENGTH)
        {
            printf("[ERROR] The message is too long. Please make it less than %d characters.\n", MAX_MSG_LENGTH);
        }
        else if (strcmp(msg_buffer, "!exit") == 0)
        {
            conversing = false;
        }
        else if(strlen(msg_buffer) > 0)
        {
            // Sending the message.
            msg_send_str(connected_to_queue, MSG_CHAT, msg_buffer);
        }

        // Handling potential messages sent to us.
        msg_fetch_pending(client_queue, &response_data, handle_pending);

        // We might've been disconnected as a result of a pending message, so 'conversing' can be false.
    }

    free(msg_buffer);

    // Disconnecting from the target client's queue.
    msg_close(connected_to_queue);
    msg_free(connected_to_queue);
    connected_to_queue = NULL;

    // Notifying the server.
    msg_send_str(server_queue, MSG_DISCONNECT, client_id_str);
}

void handle_connect_command()
{
    // Getting additional info.
    int connect_to = -1;
    scanf("%d", &connect_to);

    if (connect_to == -1)
    {
        fprintf(stderr, "Please enter a valid client id.\n");
        return;
    }

    // Sending connect request.
    sprintf(request_buffer, "%d %d", client_id, connect_to);
    msg_send_str(server_queue, MSG_CONNECT, request_buffer);

    // Waiting for response
    Data_t response_data;
    msg_wait_for_type(client_queue, &response_data, MSG_CONNECT, handle_pending);

    // Parsing the response.
    char* target_client_queue_id = response_data.buffer;

    if (strcmp(target_client_queue_id, "") == 0)
    {
        printf("Couldn't connect with " COL_CYAN "Client %d" COL_RESET ".\n", connect_to);
        return;
    }

    printf("Successfully connected with " COL_CYAN "Client %d" COL_RESET ".\n", connect_to);

    run_chat_mode(target_client_queue_id);
}

void handle_commands()
{
    static Data_t pending_data;;
    static char cmd_buffer[256];
    
    while (running)
    {
        printf("> ");
        scanf("%s", cmd_buffer);

        if (strcmp(cmd_buffer, "list") == 0)
        {
            handle_list_command();
        }
        else if(strcmp(cmd_buffer, "help") == 0)
        {
            printf("%s", HELP_STR);
        }
        else if(strcmp(cmd_buffer, "stop") == 0)
        {
            running = false;
        }
        else if(strcmp(cmd_buffer, "connect") == 0)
        {
            handle_connect_command();
        }
        else
        {
            printf("Unkown command. Type 'help' for list of commands.\n");
        }

        // Handling potential pending messages.
        msg_fetch_pending(client_queue, &pending_data, handle_pending);
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

    srand(time(NULL));

    printf("============\n");
    printf("== CLIENT ==\n");
    printf("============\n\n");

    init();

    // Flushing before setting up the listener loop.
    fflush(stdout);

    handle_commands();

    return 0;
}
