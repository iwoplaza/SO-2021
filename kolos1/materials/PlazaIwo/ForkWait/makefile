PROG=main


test: all
	@echo -e "\n\nTESTING\n\n"
	@./test.sh

clean:
	rm $(PROG)

all: main.c
	gcc -o $(PROG) -I. -Wall main.c

.PHONY: clean test
