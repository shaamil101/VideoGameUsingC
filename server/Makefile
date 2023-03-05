# Makefile for CS50 Nuggets
#   Builds and compiles all the map structs and methods into a library.
#
# Wall Street Djournal, CS50 Nuggets 23W

# object files, and the target library
OBJS = server.o
LIBS = ../mapsandvis/maps.a ../player/player.a ../support/support.a ../libcs50/libcs50-given.a 
PLAYERLIB = ../player
MAPSLIB = ../mapsandvis
SUPPORTLIB = ../support
CS50LIB = ../libcs50
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$(PLAYERLIB) -I$(MAPSLIB) -I$(SUPPORTLIB) -I$(CS50LIB)
LIBFLAGS = -lm
CC = gcc
MAKE = make

.PHONY: all server clean

all:
	make server

server: server.o
	$(CC) $(CFLAGS) server.o $(LIBS) $(LIBFLAGS) -o server

# Dependencies: object files depend on c files
server.o: server.c
	$(CC) $(CFLAGS) server.c $(LIBFLAGS) -c -o server.o

# clean up after our compilation
clean:
	rm -f server
	rm -f core
	rm -f *~ *.o
	rm -rf *.dSYM