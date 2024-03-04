CC = gcc
CFLAGS = -Wall -Wextra -pedantic -ggdb


SRC = main.c allocator.c block.c kernel.c tester.c ./avl/avl.c

.PHONY: run clean

run: main
	./main

main: $(SRC)
	$(CC) $(CFLAGS) -o main $(SRC)

clean:
	rm -f main
