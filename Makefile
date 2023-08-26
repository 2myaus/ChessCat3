CC = gcc
CFLAGS = -Wall -Wextra -fshort-enums -c
DEBUG_CFLAGS = -g

FILENAME = main.o

AR = ar
ARFLAGS = rcs
LIBNAME = libchesscat.a

EMCC = emcc
WASM_CFLAGS = -Wall -Wextra -fshort-enums -c --no-entry

WASM_FILENAME = main.o

EMAR = emar
WASM_ARFLAGS = rcs

.PHONY: all debug wasm clean

all: main

debug: CFLAGS += $(DEBUG_CFLAGS)
debug: main

wasm: CFLAGS = $(WASM_CFLAGS)
wasm: main.c misc.h
	$(EMCC) $(CFLAGS) main.c -o $(WASM_FILENAME)
	$(EMAR) $(ARFLAGS) $(LIBNAME) $(WASM_FILENAME)

main: main.c misc.h
	$(CC) $(CFLAGS) main.c -o $(FILENAME)
	$(AR) $(ARFLAGS) $(LIBNAME) $(FILENAME)

clean:
	rm -f $(FILENAME)
	rm -f $(LIBNAME)
	rm -f main.js
