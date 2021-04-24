# We specify what kind of extensions we want to clean up
CLEAN_EXTS = o a

COMMON_SRC_FILES = src/utils.c
#OPTIMIZATION = -O2
COMMON_ARGS = -Wall -I. $(OPTIMIZATION)

PRODUCER_SRC_FILES = $(COMMON_SRC_FILES) src/producer.c
CONSUMER_SRC_FILES = $(COMMON_SRC_FILES) src/consumer.c
TEST_SRC_FILES = $(COMMON_SRC_FILES) src/test.c

producer: $(PRODUCER_SRC_FILES)
	gcc -o bin/producer $(COMMON_ARGS) $(PRODUCER_SRC_FILES)

producer_debug: $(PRODUCER_SRC_FILES)
	gcc -o bin/producer $(COMMON_ARGS) $(PRODUCER_SRC_FILES) -ggdb

consumer: $(CONSUMER_SRC_FILES)
	gcc -o bin/consumer $(COMMON_ARGS) $(CONSUMER_SRC_FILES)

consumer_debug: $(CONSUMER_SRC_FILES)
	gcc -o bin/consumer $(COMMON_ARGS) $(CONSUMER_SRC_FILES) -ggdb

test_prog_debug: $(TEST_SRC_FILES) consumer_debug producer_debug
	gcc -o bin/test $(COMMON_ARGS) $(TEST_SRC_FILES) -ggdb

# For some reason necessary for CLion to work properly
all: all_producer all_producer_debug all_consumer all_consumer_debug test_prog_debug

test: all
	echo "Testing..."

clean:
	for file in $(CLEAN_EXTS); do rm -f *.$$file; done
