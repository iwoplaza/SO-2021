#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "comm.h"

#define CONN_REMOTE_STR "remote"
#define CONN_LOCAL_STR "local"
#define CONN_TYPES_STR CONN_REMOTE_STR "|" CONN_LOCAL_STR


void parse_arguments();
void run_event_loop(ClientComm_t* client);

const char* username;
ConnectionType_t connection_type;
const char* server_address;

bool running = true;

int main(int argc, char** argv)
{
    printf("[CLIENT]\n");

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

    run_event_loop(client);

    comm_client_free(client);

    return 0;
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

void run_event_loop(ClientComm_t* client)
{
    static char msg_buffer[128];

    // Sending the join request
    sprintf(msg_buffer, "%d %s", MSG_JOIN, username);
    if (!comm_send_msg(client->socket_fd, msg_buffer))
    {
        fprintf(stderr, "Failed to send JOIN request\n");
        return;
    }

    if (!comm_read_msg(client->socket_fd, msg_buffer, 128))
    {
        fprintf(stderr, "Failed to read response to JOIN request\n");
        return;
    }

    printf("Got JOIN response: %s\n", msg_buffer);

//    while (running)
//    {
//
//        int event_count = comm_wait_for_events(server, events);
//
//        for (int i = 0; i < event_count; ++i)
//        {
//            if (comm_handle_join_request(server, &events[i]))
//            {
//                // A client established a connection with us.
//            }
//            else
//            {
//                int client_fd = events[i].data.fd;
//                // A client has sent us a message.
//                char msg_buffer[256];
//                bzero(msg_buffer, 256);
//                int n = read(client_fd, msg_buffer, 255);
//
//                if (n < 0)
//                {
//                    perror("Failed to read from the socket.");
//                    continue;
//                }
//
//                MessageType_t msg_type;
//                sscanf(msg_buffer, "%d", &msg_type);
//
//                if (msg_type < 0 || msg_type >= MSG_MAX)
//                {
//                    printf("Unknown message type: %d\n", msg_type);
//                    continue;
//                }
//
//                printf("Received a %s message from %d: %s\n", MESSAGE_TYPE_NAMES[msg_type], events[i].data.fd, msg_buffer);
//
//                switch (msg_type)
//                {
//                    case MSG_PING:
//                        handle_ping_msg(msg_buffer, client_fd);
//                        break;
//                    case MSG_JOIN:
//                        handle_join_msg(msg_buffer, client_fd);
//                        break;
//                    case MSG_MOVE:
//                        handle_join_msg(msg_buffer, client_fd);
//                        break;
//                    case MSG_QUIT:
//                        handle_join_msg(msg_buffer, client_fd);
//                        break;
//                    default:
//                        fprintf(stderr, "Received message of unknown type: %d\n", msg_type);
//                        break;
//                }
//            }
//        }
//    }
}