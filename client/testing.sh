#!/bin/bash
# Unit testing invalid args bash script for client
# Wall Street Djournal - March 5th, 2023

# invalid arguments
echo no arguments:
./client
echo

echo one argument:
./client 1 2
echo

echo four arguments:
./client 1 2 3 4
echo

echo invalid server address:
./client notlocalhost randomPort
echo

echo invalid port
./client localhost foo
echo