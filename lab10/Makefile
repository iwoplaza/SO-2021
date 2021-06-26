# We specify what kind of extensions we want to clean up
CLEAN_EXTS = o a

COMMON_SRC_FILES = src/comm.c src/defs.c src/game_state.c src/msgqueue.c
SERVER_SRC_FILES = src/server.c
CLIENT_SRC_FILES = src/client.c src/client_socket.c
#OPTIMIZATION = -O2
COMMON_ARGS = -Wall -I. $(OPTIMIZATION) -lpthread

server_tcp: $(SERVER_SRC_FILES)
	gcc -o bin/server_tcp $(SERVER_SRC_FILES) ${COMMON_SRC_FILES} $(COMMON_ARGS) -DUSE_TCP

server_tcp_debug: $(SERVER_SRC_FILES)
	gcc -o bin/server_tcp $(SERVER_SRC_FILES) ${COMMON_SRC_FILES} $(COMMON_ARGS) -DUSE_TCP -ggdb

client_tcp: $(CLIENT_SRC_FILES)
	gcc -o bin/client_tcp $(CLIENT_SRC_FILES) ${COMMON_SRC_FILES} $(COMMON_ARGS) -DUSE_TCP

client_tcp_debug: $(CLIENT_SRC_FILES)
	gcc -o bin/client_tcp $(CLIENT_SRC_FILES) ${COMMON_SRC_FILES} $(COMMON_ARGS) -DUSE_TCP -ggdb

server_udp: $(SERVER_SRC_FILES)
	gcc -o bin/server_udp $(SERVER_SRC_FILES) ${COMMON_SRC_FILES} $(COMMON_ARGS)

server_udp_debug: $(SERVER_SRC_FILES)
	gcc -o bin/server_udp $(SERVER_SRC_FILES) ${COMMON_SRC_FILES} $(COMMON_ARGS) -ggdb

client_udp: $(CLIENT_SRC_FILES)
	gcc -o bin/client_udp $(CLIENT_SRC_FILES) ${COMMON_SRC_FILES} $(COMMON_ARGS)

client_udp_debug: $(CLIENT_SRC_FILES)
	gcc -o bin/client_udp $(CLIENT_SRC_FILES) ${COMMON_SRC_FILES} $(COMMON_ARGS) -ggdb

#all: server_tcp_debug client_tcp_debug server_udp_debug client_udp_debug
all: server_tcp_debug client_tcp_debug

clean:
	for file in $(CLEAN_EXTS); do rm -f *.$$file; done
