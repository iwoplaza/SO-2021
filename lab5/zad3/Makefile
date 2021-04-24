# We specify what kind of extensions we want to clean up
CLEAN_EXTS = o a

COMMON_SRC_FILES = src/utils.c
#OPTIMIZATION = -O2
COMMON_ARGS = -Wall -I. $(OPTIMIZATION)

PRODUCER_SRC_FILES = $(COMMON_SRC_FILES) src/producer.c
CONSUMER_SRC_FILES = $(COMMON_SRC_FILES) src/consumer.c

all_producer: $(PRODUCER_SRC_FILES)
	gcc -o bin/producer $(COMMON_ARGS) $(PRODUCER_SRC_FILES)

all_producer_debug: $(PRODUCER_SRC_FILES)
	gcc -o bin/producer $(COMMON_ARGS) $(PRODUCER_SRC_FILES) -ggdb

all_consumer: $(CONSUMER_SRC_FILES)
	gcc -o bin/consumer $(COMMON_ARGS) $(CONSUMER_SRC_FILES)

all_consumer_debug: $(CONSUMER_SRC_FILES)
	gcc -o bin/consumer $(COMMON_ARGS) $(CONSUMER_SRC_FILES) -ggdb


all: all_producer all_producer_debug all_consumer all_consumer_debug

test: all
	echo "Testing..."

clean:
	for file in $(CLEAN_EXTS); do rm -f *.$$file; done
