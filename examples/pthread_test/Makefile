# We specify what kind of extensions we want to clean up
CLEAN_EXTS = o a

SRC_FILES = src/main.c
#OPTIMIZATION = -O2
COMMON_ARGS = -Wall -I. $(OPTIMIZATION) -lpthread

main: $(SRC_FILES)
	gcc -o bin/main $(SRC_FILES) $(COMMON_ARGS)

main_debug: $(SRC_FILES)
	gcc -o bin/main $(SRC_FILES) $(COMMON_ARGS) -ggdb

all: main_debug

clean:
	for file in $(CLEAN_EXTS); do rm -f *.$$file; done
