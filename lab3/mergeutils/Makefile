# We specify what kind of extensions we want to clean up
CLEAN_EXTS = o a
SRC_FILES = ../src/mergeutils.c
OUTPUT_FILE_STATIC  = out/static/libmergeutils.a
OUTPUT_FILE_DYNAMIC = out/dynamic/libmergeutils.so

#DEBUG_FLAGS =  -ggdb

all: link

link: compile
	ar rcs $(OUTPUT_FILE_STATIC) build/mergeutils.o
	gcc -shared -o $(OUTPUT_FILE_DYNAMIC) build/mergeutils.o

compile:
	cd build && gcc -O -c -Wall $(SRC_FILES) -I. $(DEBUG_FLAGS)
	cd build && gcc -O -c -Wall -fpic $(SRC_FILES) -I. $(DEBUG_FLAGS)

clean:
	for file in $(CLEAN_EXTS); do rm -f *.$$file; done
