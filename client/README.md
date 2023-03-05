# CS50 Nuggets Game - client module
# Wall Street Djournal
# Winter 2023

## Client
The client module is used to interpret messages form sever and updates user interface so that user can interact to play the game

The main data structure in the client module is the client_t struct. This struct stores information such as if the client is a spectator or not, the server address, the player letter id and the player name

## Usage

* To build `client`, run `make`.
* To clean `client`, run `make clean`.

## Assumptions
The only assumption not expressed in the specs is that the max status message to be displayed is of length 80 or less.

## Testing

Clear the file `testing.out` and run `make test` in the command line.

Check `testing.out` afterwards to see the outputs of the test.
