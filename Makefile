CC = gcc
CFLAGS = -Wall -Wextra -g -fshort-enums
FILENAME = main

main: main.c misc.h
	$(CC) $(CFLAGS) main.c -o $(FILENAME)

.PHONY: clean

clean:
	rm -f $(FILENAME)
