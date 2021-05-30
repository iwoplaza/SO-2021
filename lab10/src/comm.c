#include "comm.h"
#define _SBID_SOURCE
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/epoll.h>
#include <errno.h>
#include "defs.h"

#ifdef USE_TCP
    #define SOCK_TYPE SOCK_STREAM
#else
    #define SOCK_TYPE SOCK_DGRAM
#endif

#define MAX_EVENTS 16

int _create_socket(int domain, const char* label)
{
    int fd = socket(domain, SOCK_TYPE, 0);
    if (fd == -1)
    {
        fprintf(stderr, "Failed to create socket: %s\n", label);
        perror("Error details");
        exit(1);
    }

    return fd;
}

void _bind_local_server_address(int socket_fd)
{
    struct sockaddr_un server_addr;

    // Make sure the address we're planning to use isn't too long.
    unsigned int max_path_len = sizeof(server_addr.sun_path);
    if (strlen(LOCAL_SOCKET_PATH) >= max_path_len)
    {
        fprintf(stderr, "Local socket path too long: %s. Max length is %d", LOCAL_SOCKET_PATH, max_path_len);
        exit(1);
    }

    if (remove(LOCAL_SOCKET_PATH) == -1 && errno != ENOENT)
    {
        perror("Couldn't remove existing local socket file");
        exit(1);
    }

    // Zeroing out the address buffer.
    bzero((char*) &server_addr, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, LOCAL_SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (bind(socket_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) != 0)
    {
        perror("Failed to bind socket");
        exit(1);
    }
}

void _bind_online_server_address(int socket_fd, int port)
{
    struct sockaddr_in addr;

    // Zeroing out the address buffer.
    bzero((char*) &addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_fd, (struct sockaddr*) &addr, sizeof(addr)) != 0)
    {
        perror("Failed to bind online socket");
        exit(1);
    }
}

ServerComm_t* comm_server_init(int port)
{
    int local_fd = _create_socket(AF_UNIX, "local");
    _bind_local_server_address(local_fd);

    int online_fd = _create_socket(AF_INET, "online");
    _bind_online_server_address(online_fd, port);

    // Preparing the sockets to listen for client connections.
    // To not reject any clients unnecessarily, I set the queue size to MAX_CLIENTS.
    listen(local_fd, MAX_CLIENTS);
    listen(online_fd, MAX_CLIENTS);

    int epoll_fd = epoll_create1(0);

    struct epoll_event ev;
    ev.data.fd = local_fd;
    ev.events = EPOLLIN;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, local_fd, &ev) < 0)
    {
        fprintf(stderr, "Failed to add local socket to event pool.\n");
        exit(1);
    }

    ev.data.fd = online_fd;
    ev.events = EPOLLIN;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, online_fd, &ev) < 0)
    {
        fprintf(stderr, "Failed to add online socket to event pool.\n");
        exit(1);
    }

    ServerComm_t* comm = malloc(sizeof(ServerComm_t));

    comm->epoll_fd = epoll_fd;
    comm->local_fd = local_fd;
    comm->online_fd = online_fd;

    return comm;
}

void comm_server_free(ServerComm_t* comm)
{
    close(comm->local_fd);
    close(comm->online_fd);
    close(comm->epoll_fd);
    free(comm);
}

void comm_msg_loop(ServerComm_t* server)
{
    // Client address
    struct sockaddr_in client_addr;
    socklen_t clilen = sizeof(client_addr);

    struct epoll_event ev;
    struct epoll_event events[MAX_EVENTS];

    while (true)
    {
        // Waiting for events
        printf("Waiting for events...\n");
        int nfds = epoll_wait(server->epoll_fd, events, MAX_EVENTS, -1);

        for (int i = 0; i < nfds; ++i)
        {
            if (events[i].data.fd == server->local_fd)
            {
                int client = accept(server->local_fd, (struct sockaddr*) &client_addr, &clilen);

                if (client < 0)
                {
                    perror("Couldn't accept client.");
                    continue;
                }

                ev.events = EPOLLIN;
                ev.data.fd = client;

                if (epoll_ctl(server->epoll_fd, EPOLL_CTL_ADD, client, &ev) < 0)
                {
                    fprintf(stderr, "Failed to add client %d to event pool.\n", client);
                    exit(1);
                }

                printf("Added client %d.\n", client);
            }
            else
            {
                printf("Eyyy, we got something from a client.");

                char buffer[256];
                bzero(buffer, 256);
                int n = read(events[i].data.fd, buffer, 255);

                if (n < 0)
                {
                    perror("Failed to read from the socket.");
                    return;
                }

                printf("Here's the message: %s\n", buffer);

                n = write(events[i].data.fd, "I got your message", 18);

                if (n < 0)
                {
                    perror("Failed to write to the socket");
                    return;
                }
            }
        }
    }
}

void connect_client(int epoll_fd)
{
    int client_fd = 0;

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP;
    // This is gonna be sent back to us in the event handler.
    // Using this to identify to which client the event applies.
    event.data.fd = client_fd;

    // Registering client
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1)
    {
        perror("Failed to register client connection to the polling system.");
        return;
    }

    struct epoll_event events[16];

    // Waiting for event
    epoll_wait(epoll_fd, events, 16, 5000);
}

ClientComm_t* comm_client_init(const char* server_address)
{
    struct in_addr inp;

    if (inet_aton(server_address, &inp) == 0)
    {
        perror("Failed to parse server ip address");
        return NULL;
    }

    ClientComm_t* comm = malloc(sizeof(ClientComm_t));

    return comm;
}

void comm_client_free(ClientComm_t* comm)
{
    free(comm);
}
