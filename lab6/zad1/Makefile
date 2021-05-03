# We specify what kind of extensions we want to clean up
CLEAN_EXTS = o a

COMMON_SRC_FILES = src/queue_systemv.c src/defs.c src/utils.c
#OPTIMIZATION = -O2
COMMON_ARGS = -Wall -I. $(OPTIMIZATION)

SERVER_SRC_FILES = $(COMMON_SRC_FILES) src/server.c
CLIENT_SRC_FILES = $(COMMON_SRC_FILES) src/client.c

server: $(SERVER_SRC_FILES)
	gcc -o bin/server $(COMMON_ARGS) $(SERVER_SRC_FILES)

server_debug: $(SERVER_SRC_FILES)
	gcc -o bin/server $(COMMON_ARGS) $(SERVER_SRC_FILES) -ggdb

client: $(CLIENT_SRC_FILES)
	gcc -o bin/client $(COMMON_ARGS) $(CLIENT_SRC_FILES)

client_debug: $(CLIENT_SRC_FILES)
	gcc -o bin/client $(COMMON_ARGS) $(CLIENT_SRC_FILES) -ggdb

all: server_debug client_debug

clean:
	for file in $(CLEAN_EXTS); do rm -f *.$$file; done
