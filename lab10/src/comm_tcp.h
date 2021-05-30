#ifndef LAB10_COMM_TCP_H
#define LAB10_COMM_TCP_H

typedef struct ServerComm_t {
    int online_fd;
    int local_fd;
    int epoll_fd;
} ServerComm_t;

typedef struct ClientComm_t {
    int socket_fd;
} ClientComm_t;

#endif //LAB10_COMM_TCP_H
