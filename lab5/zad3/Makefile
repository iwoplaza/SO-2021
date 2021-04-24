# We specify what kind of extensions we want to clean up
CLEAN_EXTS = o a

COMMON_SRC_FILES = src/utils.c
#OPTIMIZATION = -O2
COMMON_ARGS = -Wall -I. $(OPTIMIZATION)

PRODUCER_SRC_FILES = $(COMMON_SRC_FILES) src/producer.c
CONSUMER_SRC_FILES = $(COMMON_SRC_FILES) src/consumer.c src/aggregate.c
TEST_SRC_FILES = $(COMMON_SRC_FILES) src/test.c src/aggregate.c

producer: $(PRODUCER_SRC_FILES)
	gcc -o bin/producer $(COMMON_ARGS) $(PRODUCER_SRC_FILES)

producer_debug: $(PRODUCER_SRC_FILES)
	gcc -o bin/producer $(COMMON_ARGS) $(PRODUCER_SRC_FILES) -ggdb

consumer: $(CONSUMER_SRC_FILES)
	gcc -o bin/consumer $(COMMON_ARGS) $(CONSUMER_SRC_FILES)

consumer_debug: $(CONSUMER_SRC_FILES)
	gcc -o bin/consumer $(COMMON_ARGS) $(CONSUMER_SRC_FILES) -ggdb

test_prog: $(TEST_SRC_FILES) consumer producer
	gcc -o bin/test $(COMMON_ARGS) $(TEST_SRC_FILES)

test_prog_debug: $(TEST_SRC_FILES) consumer_debug producer_debug
	gcc -o bin/test $(COMMON_ARGS) $(TEST_SRC_FILES) -ggdb

all: producer_debug consumer_debug test_prog_debug

test: test_prog
	cd bin && ./test 5 1 /home/iwo/potok out_test.txt 5
	cd bin && ./test 1 5 /home/iwo/potok out_test.txt 5
	cd bin && ./test 5 5 /home/iwo/potok out_test.txt 5

	cd bin && ./test 5 1 /home/iwo/potok out_test.txt 1000
	cd bin && ./test 1 5 /home/iwo/potok out_test.txt 1000
	cd bin && ./test 5 5 /home/iwo/potok out_test.txt 1000

	cd bin && ./test 5 1 /home/iwo/potok out_test.txt 5000
	cd bin && ./test 1 5 /home/iwo/potok out_test.txt 5000
	cd bin && ./test 5 5 /home/iwo/potok out_test.txt 5000

clean:
	for file in $(CLEAN_EXTS); do rm -f *.$$file; done
