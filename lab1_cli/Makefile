# We specify what kind of extensions we want to clean up
CLEAN_EXTS = o a
SRC_FILES = src/main.c src/timekeeping.c src/command_merge_files.c \
	src/command_remove_block.c src/command_remove_row.c \
	src/command_create_table.c src/command_save_block.c \
	src/command_print_block.c src/command_begin_report.c
REPORT_OUT_FILE = report.txt

MULIB_INCLUDE_DIR = ../lab1/src
MULIB_LINK_DIR = ../lab1/out

#DEBUG_FLAGS =  -ggdb
MEASURE_TIME_FLAGS = -D MEASURE_TIME
OPTIMIZATION = -O2

COMMON_ARGS = -o bin/lab1_cli -Wall $(OPTIMIZATION) $(SRC_FILES) $(DEBUG_FLAGS) $(MEASURE_TIME_FLAGS)

all: $(SRC_FILES)
	gcc $(COMMON_ARGS) -I. -I$(MULIB_INCLUDE_DIR) -L$(MULIB_LINK_DIR)/static -llab1

all_dynamic: $(SRC_FILES)
	gcc $(COMMON_ARGS) -I. -I$(MULIB_INCLUDE_DIR) -L$(MULIB_LINK_DIR)/dynamic -llab1  -Wl,-rpath=./

all_dynamic_loaded: $(SRC_FILES)
	gcc $(COMMON_ARGS) -I. -D DYN_LOAD -ldl -Wl,-rpath=./

test: all_dynamic_loaded
	cd bin && echo "begin_report $(REPORT_OUT_FILE) \
create_table 1 merge_files a_s.txt:b_s.txt merge_files a_m.txt:b_m.txt merge_files a_l.txt:b_l.txt \
save_block 0 out.txt save_block 1 out.txt save_block 2 out.txt \
remove_block 0 remove_block 0 remove_block 0 exit" | ./lab1_cli

clean:
	for file in $(CLEAN_EXTS); do rm -f *.$$file; done
