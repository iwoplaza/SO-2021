# We specify what kind of extensions we want to clean up
CLEAN_EXTS = o a

# COMMON_SRC_FILES = 
COMMON_ARGS = -Wall -I.
OPTIMIZATION = -O2

SRC_FILES = $(COMMON_SRC_FILES) src/main.c

program:
	gcc -o bin/client $(COMMON_ARGS) $(CLIENT_SRC_FILES) $(OPTIMIZATION)

program_debug:
	gcc -o bin/client $(COMMON_ARGS) $(CLIENT_SRC_FILES) -ggdb

all: program

clean:
	for file in $(CLEAN_EXTS); do rm -f *.$$file; done
