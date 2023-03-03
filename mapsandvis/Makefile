# Makefile for CS50 Nuggets maps
#   Builds and compiles all the map structs and methods into a library.
#
# Wall Street Djournal, CS50 Nuggets 23W

# object files, and the target library
OBJS = maps.a
LIBS = ../libcs50/libcs50-given.a  ../support/support.a ../player/player.a
LLIBS = -L../libcs50 -L../support -L../player
PLAYERLIB = ../player
SUPPORTLIB = ../support
CFLAGS = -Wall -pedantic -std=c11 -ggdb $(LLIBS) $(LIBS) -I$(PLAYERLIB) -I$(SUPPORTLIB) -I../libcs50 $(FLAGS)
CC = gcc
MAKE = make

.PHONY: all test clean

all: maps.o
	ar cr maps.a maps.o

# Dependencies: object files depend on header files
maps.o: maps.h

# clean up after our compilation
clean:
	rm -f core
	rm -f $(LIB) *~ *.o