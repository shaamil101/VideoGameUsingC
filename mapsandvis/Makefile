# Makefile for CS50 Nuggets maps
#   Builds and compiles all the map structs and methods into a library.
#
# Wall Street Djournal, CS50 Nuggets 23W

# object files, and the target library
OBJS = maps.o
LIBS = -lm
PLAYERLIB = ../player/player.h
CFLAGS = -Wall -pedantic -std=c11 -ggdb $(LIBS) -I$(PLAYERLIB) $(FLAGS)
CC = gcc
MAKE = make

.PHONY: all test clean

all: $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o maps

# Dependencies: object files depend on header files
maps.o: maps.h

# clean up after our compilation
clean:
	rm -f core
	rm -f $(LIB) *~ *.o