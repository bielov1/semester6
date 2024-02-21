CC = gcc
CFLAGS = -Wall -Wextra -pedantic

SRC = main.c allocator.c block.c kernel.c

.PHONY: run clean

run: main
	./main

main: $(SRC)
	$(CC) $(CFLAGS) -o main $(SRC)

clean:
	rm -f main
