CC = gcc
CFLAGS = -Wall -Wextra -g -fshort-enums
LFLAGS = -L .. -lchesscat
FILENAME = demo

main: demo.c
	$(CC) $(CFLAGS) demo.c $(LFLAGS) -o $(FILENAME)

.PHONY: clean

clean:
	rm -f $(FILENAME)