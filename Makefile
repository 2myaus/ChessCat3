CC = gcc
CFLAGS = -Wall -Wextra -g -fshort-enums

main: main.c misc.h
	$(CC) $(CFLAGS) main.c -o main

.PHONY: clean

clean:
	rm -f main
