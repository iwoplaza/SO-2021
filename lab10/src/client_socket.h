#ifndef LAB10_CLIENT_SOCKET_H
#define LAB10_CLIENT_SOCKET_H

#include <pthread.h>
#include "comm.h"
#include "game_state.h"

typedef struct SocketThreadArgs_t {
    ClientComm_t* client;
    GameState_t* game_state;
    bool running;
} SocketThreadArgs_t;

pthread_t init_sockets(SocketThreadArgs_t* args, const char* username);

#endif //LAB10_CLIENT_SOCKET_H