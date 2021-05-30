#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "comm.h"


void parse_arguments();

int port;
const char* socket_path;

int main(int argc, char** argv)
{
    printf("[SERVER]\n");

    if (argc != 3)
    {
        printf("Usage: <port> <socket_path>");
        return 0;
    }

    parse_arguments(argc, argv);

    ServerComm_t* server = comm_server_init(5001);

    if (server == NULL)
    {
        return 1;
    }

    comm_msg_loop(server);

    comm_server_free(server);

    return 0;
}

void parse_arguments(int argc, char** argv)
{
    port = atoi(argv[1]);
    socket_path = argv[2];
}