#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "comm.h"

#define CONN_REMOTE_STR "remote"
#define CONN_LOCAL_STR "local"
#define CONN_TYPES_STR CONN_REMOTE_STR "|" CONN_LOCAL_STR


void parse_arguments();

const char* username;
const ConnectionType_t connection_type;
const char* server_address;

int main(int argc, char** argv)
{
    printf("[CLIENT]\n");

    if (argc != 4)
    {
        printf("Usage: <username> (" CONN_TYPES_STR ") <address>");
        return 0;
    }

    parse_arguments(argc, argv);

    return 0;
}

void parse_arguments(int argc, char** argv)
{
    username = argv[1];

    const char* conn_type_name = argv[2];
    if (strcmp(conn_type_name, CONN_REMOTE_STR))
    {
        connection_type = CONNECTION_REMOTE;
    }
    else if (strcmp(conn_type_name, CONN_LOCAL_STR))
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