# We specify what kind of extensions we want to clean up
CLEAN_EXTS = o a

SRC_FILES = src/main.c
#OPTIMIZATION = -O2
COMMON_ARGS = -Wall -I. $(OPTIMIZATION) -lpthread

main: $(CHEF_SRC_FILES)
	gcc -o bin/main $(COMMON_ARGS) $(SRC_FILES)

main_debug: $(CHEF_SRC_FILES)
	gcc -o bin/main $(COMMON_ARGS) $(SRC_FILES) -ggdb

all: main_debug

clean:
	for file in $(CLEAN_EXTS); do rm -f *.$$file; done
