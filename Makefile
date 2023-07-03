CC = gcc
CFLAGS = -Wall -Wextra -g -fshort-enums -c
FILENAME = main.o

main: main.c misc.h
	$(CC) $(CFLAGS) main.c -o $(FILENAME)

.PHONY: clean

clean:
	rm -f $(FILENAME)
