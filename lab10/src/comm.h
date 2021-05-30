#ifndef LAB10_COMM_H
#define LAB10_COMM_H

typedef enum ConnectionType_t {
    UNKNOWN,
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
ServerComm_t* comm_server_init(int port);
void comm_msg_loop(ServerComm_t* server);
void comm_server_free(ServerComm_t* comm);

// Client
ClientComm_t* comm_client_init();
void comm_client_free(ClientComm_t* comm);

#endif //LAB10_COMM_H
