# Makefile for CS50 Nuggets maps
#   Builds and compiles all the map structs and methods into a library.
#
# Wall Street Djournal, CS50 Nuggets 23W

# object files, and the target library
OBJS = maps.a mapstest.o
LIBS = ../player/player.a ../support/support.a ../libcs50/libcs50-given.a
PLAYERLIB = ../player
SUPPORTLIB = ../support
CS50LIB = ../libcs50
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$(PLAYERLIB) -I$(SUPPORTLIB) -I$(CS50LIB)
LIBFLAGS = $(LIBS) -lm
CC = gcc
MAKE = make

.PHONY: all maps.a mapstest clean

all: maps.a mapstest

maps.a: maps.o
	ar cr maps.a maps.o

mapstest: mapstest.o maps.a
# $(CC) $(CFLAGS) mapstest.c maps.a ../player/player.a ../support/support.a ../libcs50/libcs50-given.a -I../player -I../support -lm -o mapstest
	$(CC) $(CFLAGS) $(LIBFLAGS) mapstest.c maps.a $(LIBFLAGS) -o mapstest

# Dependencies: object files depend on header files
maps.o: maps.h

# clean up after our compilation
clean:
	rm -f mapstest
	rm -f maps.a
	rm -f core
	rm -f $(LIB) *~ *.o