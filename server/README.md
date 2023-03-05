# Server - CS50 Nuggets
The server that handles game logic and game system for the cs50 nuggets project

Wall Street Djournal - CS50 Nuggets 23W

## Running instructions
To start the server:
 ```
./server [map.txt] [optional seed parameter] [optional bot number]
```

## Compilation instructions
Clean the directory with `make clean` and compile the server with `make	`

## Testing plan 

We have a servertest bash script that tests the server for invalid arguments and then we can run one of two different tests.
Before that however, we must run the server with the small.txt map and seed 123 for the pre-determined input steps to work.
    1. First option to run is by running a bot on our server
    2. Second, we can test our server by inputting a series of commands by mimicking the standard input and find the output in the server logging.

## Implementation notes
We follow our implementation spec closely while writing the server, please refer to that as it is outlined in the main directory.
There are no known deviations from spec.

