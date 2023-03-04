# Makefile for CS50 Nuggets 
#   Builds and compiles everything together
#
# Wall Street Djournal, CS50 Nuggets 23W

# object files, and the target library
OBJS = server.o client.o
LIBS = ../mapsandvis/maps.a ../player/player.a ../support/support.a ../libcs50/libcs50-given.a
PLAYERLIB = ../player
SUPPORTLIB = ../support
MAPSLIB = ../mapsandvis
CS50LIB = ../libcs50
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$(MAPSLIB) -I$(PLAYERLIB) -I$(SUPPORTLIB) -I$(CS50LIB)
LIBFLAGS = $(LIBS) -lm
CC = gcc
MAKE = make

.PHONY: all server client clean

all:
	make player
	make maps
	make server
	make client

server:
	make -C server

client:
	make -C client

player:
	make -C player

maps:
	make -C mapsandvis


# clean up after our compilation
clean:
	make -C server clean
	make -C client clean
	make -C mapsandvis clean
	make -C player clean
	rm -f core
	rm -f $(LIB) *~ *.o