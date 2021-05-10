# We specify what kind of extensions we want to clean up
CLEAN_EXTS = o a

COMMON_SRC_FILES = src/utils.c
SYSTEMV_SRC_FILES =src/helper_systemv.c \
	-DLAB7_SYSTEMV
POSIX_SRC_FILES = src/helper_posix.c -lrt -lpthread \
	-DLAB7_POSIX \
	-DLAB7_MAIN_NAME="\"main_posix\"" \
	-DLAB7_CHEF_NAME="\"chef_posix\"" \
	-DLAB7_DELIVERY_NAME="\"delivery_posix\"" \
#OPTIMIZATION = -O2
COMMON_ARGS = -Wall -I. $(OPTIMIZATION)

MAIN_SRC_FILES = $(COMMON_SRC_FILES) src/main.c
CHEF_SRC_FILES = $(COMMON_SRC_FILES) src/chef.c
DELIVERY_SRC_FILES = $(COMMON_SRC_FILES) src/delivery.c

# System V

main_systemv: $(MAIN_SRC_FILES)
	gcc -o bin/main_systemv $(COMMON_ARGS) $(MAIN_SRC_FILES) $(SYSTEMV_SRC_FILES) -lm

chef_systemv: $(CHEF_SRC_FILES)
	gcc -o bin/chef_systemv $(COMMON_ARGS) $(CHEF_SRC_FILES) $(SYSTEMV_SRC_FILES) -lm

chef_systemv_debug: $(CHEF_SRC_FILES)
	gcc -o bin/chef_systemv $(COMMON_ARGS) $(CHEF_SRC_FILES) $(SYSTEMV_SRC_FILES) -ggdb -lm

delivery_systemv: $(DELIVERY_SRC_FILES)
	gcc -o bin/delivery_systemv $(COMMON_ARGS) $(DELIVERY_SRC_FILES) $(SYSTEMV_SRC_FILES) -lm

delivery_systemv_debug: $(DELIVERY_SRC_FILES)
	gcc -o bin/delivery_systemv $(COMMON_ARGS) $(DELIVERY_SRC_FILES) $(SYSTEMV_SRC_FILES) -ggdb -lm

# Posix

main_posix: $(MAIN_SRC_FILES)
	gcc -o bin/main_posix $(COMMON_ARGS) $(MAIN_SRC_FILES) $(POSIX_SRC_FILES) -lm

chef_posix: $(CHEF_SRC_FILES)
	gcc -o bin/chef_posix $(COMMON_ARGS) $(CHEF_SRC_FILES) $(POSIX_SRC_FILES) -lm

chef_posix_debug: $(CHEF_SRC_FILES)
	gcc -o bin/chef_posix $(COMMON_ARGS) $(CHEF_SRC_FILES) $(POSIX_SRC_FILES) -ggdb -lm

delivery_posix: $(DELIVERY_SRC_FILES)
	gcc -o bin/delivery_posix $(COMMON_ARGS) $(DELIVERY_SRC_FILES) $(POSIX_SRC_FILES) -lm

delivery_posix_debug: $(DELIVERY_SRC_FILES)
	gcc -o bin/delivery_posix $(COMMON_ARGS) $(DELIVERY_SRC_FILES) $(POSIX_SRC_FILES) -ggdb -lm

# General

all: main_systemv chef_systemv_debug delivery_systemv_debug main_posix chef_posix_debug delivery_posix_debug

clean:
	for file in $(CLEAN_EXTS); do rm -f *.$$file; done
