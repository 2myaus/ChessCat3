CC = gcc
CFLAGS = -Wall -Wextra -g -fshort-enums -c
FILENAME = main.o

AR = ar
ARFLAGS = rcs
LIBNAME = libchesscat.a

main: main.c misc.h
	$(CC) $(CFLAGS) main.c -o $(FILENAME)
	$(AR) $(ARFLAGS) $(LIBNAME) $(FILENAME)

.PHONY: clean

clean:
	rm -f $(FILENAME)
	rm -f $(LIBNAME)
