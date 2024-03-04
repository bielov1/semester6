CC = gcc
CFLAGS = -Wall -Wconversion -Wextra -pedantic -ggdb


SRC = main.c allocator.c block.c kernel.c tester.c ./avl/avl.c

.PHONY: run clean

run: main
	./main

main: $(SRC)
	$(CC) $(CFLAGS) -o main $(SRC)

clean:
	rm -rf ./main
