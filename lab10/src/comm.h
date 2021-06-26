#ifndef LAB10_COMM_H
#define LAB10_COMM_H

#include <stdbool.h>
#include <sys/epoll.h>
#include "defs.h"
#include "msgqueue.h"

#define MAX_EVENTS 16

typedef enum ConnectionType_t {
    CONNECTION_REMOTE,
    CONNECTION_LOCAL
} ConnectionType_t;

#ifdef USE_TCP
    #include "comm_tcp.h"
#elif defined(USE_UDP)
    #include "comm_udp.h"
#else
    typedef struct ServerComm_t {} ServerComm_t;
    typedef struct ClientComm_t {} ClientComm_t;
    // #error A type of communication has to be specified, either USE_TCP or USE_UDP
#endif

// Server
ServerComm_t* comm_server_init(int port, const char* socket_path);
void comm_msg_loop(ServerComm_t* server);
int comm_wait_for_events(ServerComm_t* server, struct epoll_event events[MAX_EVENTS]);
void comm_server_free(ServerComm_t* comm);
bool comm_handle_join_request(ServerComm_t* server, struct epoll_event* join_ev);

// Client
ClientComm_t* comm_client_init_local(const char* socket_path);
ClientComm_t* comm_client_init_remote();
void comm_client_free(ClientComm_t* comm);

// Common
bool comm_send_msg(int fd, const char* contents);
bool comm_read_msgs(int fd, MessageQueue_t* queue);
MessageType_t get_and_validate_msg_type(char* msg_buffer);
void expect_msg_type(char* msg_buffer, MessageType_t msg_type);
#endif //LAB10_COMM_H
