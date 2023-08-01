CC = gcc
CFLAGS = -Wall -Wextra -fshort-enums -c
DEBUG_CFLAGS = -g

FILENAME = main.o

AR = ar
ARFLAGS = rcs
LIBNAME = libchesscat.a

.PHONY: all debug clean

all: main

debug: CFLAGS += $(DEBUG_CFLAGS)
debug: main

main: main.c misc.h
	$(CC) $(CFLAGS) main.c -o $(FILENAME)
	$(AR) $(ARFLAGS) $(LIBNAME) $(FILENAME)

clean:
	rm -f $(FILENAME)
	rm -f $(LIBNAME)
