# We specify what kind of extensions we want to clean up
CLEAN_EXTS = o a

COMMON_SRC_FILES = src/defs.c src/utils.c
SYSTEMV_SRC_FILES = src/queue_systemv.c
POSIX_SRC_FILES = src/queue_posix.c -lrt
#OPTIMIZATION = -O2
COMMON_ARGS = -Wall -I. $(OPTIMIZATION)

SERVER_SRC_FILES = $(COMMON_SRC_FILES) src/server.c
CLIENT_SRC_FILES = $(COMMON_SRC_FILES) src/client.c

# System V

server_systemv: $(SERVER_SRC_FILES)
	gcc -o bin/server_systemv $(COMMON_ARGS) $(SERVER_SRC_FILES) $(SYSTEMV_SRC_FILES)

server_systemv_debug: $(SERVER_SRC_FILES)
	gcc -o bin/server_systemv $(COMMON_ARGS) $(SERVER_SRC_FILES) $(SYSTEMV_SRC_FILES) -ggdb

client_systemv: $(CLIENT_SRC_FILES)
	gcc -o bin/client_systemv $(COMMON_ARGS) $(CLIENT_SRC_FILES) $(SYSTEMV_SRC_FILES)

client_systemv_debug: $(CLIENT_SRC_FILES)
	gcc -o bin/client_systemv $(COMMON_ARGS) $(CLIENT_SRC_FILES) $(SYSTEMV_SRC_FILES) -ggdb

# Posix

server_posix: $(SERVER_SRC_FILES)
	gcc -o bin/server_posix $(COMMON_ARGS) $(SERVER_SRC_FILES) $(POSIX_SRC_FILES)

server_posix_debug: $(SERVER_SRC_FILES)
	gcc -o bin/server_posix $(COMMON_ARGS) $(SERVER_SRC_FILES) $(POSIX_SRC_FILES) -ggdb

client_posix: $(CLIENT_SRC_FILES)
	gcc -o bin/client_posix $(COMMON_ARGS) $(CLIENT_SRC_FILES) $(POSIX_SRC_FILES)

client_posix_debug: $(CLIENT_SRC_FILES)
	gcc -o bin/client_posix $(COMMON_ARGS) $(CLIENT_SRC_FILES) $(POSIX_SRC_FILES) -ggdb

# General

all: server_systemv_debug client_systemv_debug server_posix_debug client_posix_debug

clean:
	for file in $(CLEAN_EXTS); do rm -f *.$$file; done
