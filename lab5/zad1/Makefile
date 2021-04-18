# We specify what kind of extensions we want to clean up
CLEAN_EXTS = o a
SRC_FILES = src/main.c src/utils.c src/lang.c

#OPTIMIZATION = -O2

COMMON_ARGS = -o bin/zad1 -Wall -I. $(OPTIMIZATION) $(SRC_FILES)

all: $(SRC_FILES)
	gcc $(COMMON_ARGS)

all_debug: $(SRC_FILES)
	gcc $(COMMON_ARGS) -ggdb

test: all
	echo "Testing..."

clean:
	for file in $(CLEAN_EXTS); do rm -f *.$$file; done
