# Makefile for Nuggets client module
#
# Wall Street DJournal - Jack Desmond - February 1, 2023

LLIBS = ../support/support.a ../libcs50/libcs50-given.a
LIBS = -lncurses
CFLAGS =  -Wall -pedantic -std=c11 -ggdb -I../support -I../libcs50
CC = gcc
MAKE = make

.PHONY: all clean

all: client

client: client.o $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o client

client.o: client.c ../support/message.h ../support/log.h ../libcs50/mem.h

../support/support.a:
	make -C ../support support.a

clean:
	make -C $S clean
	rm -f *~ *.o core