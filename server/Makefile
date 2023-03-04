# Makefile for CS50 Nuggets maps
#   Builds and compiles all the map structs and methods into a library.
#
# Wall Street Djournal, CS50 Nuggets 23W

# object files, and the target library
OBJS = server.o
LIBS = ../player/player.a ../support/support.a ../libcs50/libcs50-given.a ../mapsandvis/maps.a
PLAYERLIB = ../player
MAPSLIB = ../mapsandvis
SUPPORTLIB = ../support
CS50LIB = ../libcs50
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$(PLAYERLIB) -I$(MAPSLIB) -I$(SUPPORTLIB) -I$(CS50LIB)
LIBFLAGS = $(LIBS) -lm
CC = gcc
MAKE = make

.PHONY: all server clean

all: server

server: mapstest.o maps.a
# $(CC) $(CFLAGS) mapstest.c maps.a ../player/player.a ../support/support.a ../libcs50/libcs50-given.a -I../player -I../support -lm -o mapstest
	$(CC) $(CFLAGS) $(LIBFLAGS) server.c $(LIBFLAGS) -o server

# Dependencies: object files depend on header files
server.o: server.h

# clean up after our compilation
clean:
	rm -f server
	rm -f core
	rm -f *~ *.o
	rm -rf *.dSYM