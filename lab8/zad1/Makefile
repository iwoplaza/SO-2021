# We specify what kind of extensions we want to clean up
CLEAN_EXTS = o a

SRC_FILES = src/main.c src/pgm.c src/region.c src/timeutil.c
#OPTIMIZATION = -O2
COMMON_ARGS = -Wall -I. $(OPTIMIZATION) -lpthread

main: $(CHEF_SRC_FILES)
	gcc -o bin/main $(SRC_FILES) $(COMMON_ARGS)

main_debug: $(CHEF_SRC_FILES)
	gcc -o bin/main $(SRC_FILES) $(COMMON_ARGS) -ggdb

all: main_debug

clean:
	for file in $(CLEAN_EXTS); do rm -f *.$$file; done
