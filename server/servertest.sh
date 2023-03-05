#!/bin/bash

# zero arguments
./server
# three arguments
./server fakearg fakearg2 fakearg3
# four arguments
./server fakemap fakearg2 fakearg3 fakearg4
# two arguments but with a non-existent map
./server ../DNE 123
# two arguments but with a false seed which needs to be an integer
./server ../maps/small.txt word

#Running a bot with the portnumber entered manually. First you must run ./server ../maps/small.txt 123
PORTNUM1=38031
#~/cs50-dev/shared/nuggets/linux/client plank $PORTNUM1 bot 2> /dev/null

../support/miniclient localhost $PORTNUM1 < StdinForServer.txt 