#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "logs.h"
#include "defs.h"
#include "comm.h"
#include "game_state.h"
#include "client_socket.h"

#define CONN_REMOTE_STR "remote"
#define CONN_LOCAL_STR "local"
#define CONN_TYPES_STR CONN_REMOTE_STR "|" CONN_LOCAL_STR

const char* HELP_STR = \
    COL_YELLOW "move <col> <row>" COL_RESET " - Performs a move on a specific cell.\n" \
    COL_YELLOW "stop            " COL_RESET " - Closes the client.\n";


static void handle_shutdown();
static void handle_commands();
static void parse_arguments();

const char* username;
ConnectionType_t connection_type;
const char* server_address;

bool cli_running = true;
bool in_game = false;
GameState_t game_state;

void handle_interrupt()
{
    exit(0);
}

int main(int argc, char** argv)
{
    signal(SIGINT, handle_interrupt);
    atexit(handle_shutdown);
    
    printf("============\n");
    printf("== CLIENT ==\n");
    printf("============\n\n");

    if (argc != 4)
    {
        printf("Usage: <username> (" CONN_TYPES_STR ") <address|path>\n");
        return 0;
    }

    parse_arguments(argc, argv);

    ClientComm_t* client = NULL;

    if (connection_type == CONNECTION_LOCAL)
    {
        client = comm_client_init_local(server_address);
    }

    if (client == NULL)
    {
        return -1;
    }

    reset_game_state(&game_state);

    SocketThreadArgs_t* thread_args = malloc(sizeof(SocketThreadArgs_t));
    thread_args->client = client;
    thread_args->game_state = &game_state;
    thread_args->running = true;

    pthread_t socket_thread = init_sockets(thread_args, username);

    if (socket_thread == -1)
    {
        return 1;
    }

    handle_commands();

    pthread_join(socket_thread, NULL);

    comm_client_free(client);

    return 0;
}

void handle_shutdown()
{
    printf("Shutting down...\n");

    exit(0);
}

void parse_arguments(int argc, char** argv)
{
    username = argv[1];

    const char* conn_type_name = argv[2];
    if (strcmp(conn_type_name, CONN_REMOTE_STR) == 0)
    {
        connection_type = CONNECTION_REMOTE;
    }
    else if (strcmp(conn_type_name, CONN_LOCAL_STR) == 0)
    {
        connection_type = CONNECTION_LOCAL;
    }
    else
    {
        fprintf(stderr, "Unsupported connection type: '%s'. Choose from: %s\n", conn_type_name, CONN_TYPES_STR);
        exit(1);
    }

    server_address = argv[3];
}

void handle_move_cmd()
{
    printf("Moving...\n");

    

    printf("Waiting for a move...");
}

void handle_commands()
{
    static char cmd_buffer[256];
    
    print_game_state(&game_state);

    while (cli_running)
    {
        printf("> ");
        scanf("%s", cmd_buffer);

        if (strcmp(cmd_buffer, "help") == 0)
        {
            printf("%s", HELP_STR);
        }
        else if (strcmp(cmd_buffer, "move") == 0)
        {
            handle_move_cmd();
        }
        else if (strcmp(cmd_buffer, "stop") == 0)
        {
            cli_running = false;
        }
        else
        {
            printf("Unkown command. Type 'help' for list of commands.\n");
        }
    }
}
