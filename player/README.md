# CS50 Nuggets Game - player module
# Wall Street Djournal
# Winter 2023

## Player
The player module will have the data structure that the server program will use to store and access information about a player.

The main data structure in the player module will be the player_t struct. This structure will contain either pointers to the stored values or if they are integers, the value of the integer. 

## Usage

* To build `player`, run `make`.
* To clean `player`, run `make clean`.

## Assumptions & Comments 
There are no extra assumptions besides those laid out in the specs. See the specs for all details.

## Testing

For testing the player module, because it is a minor data structure that is utilized by the server module, the testing for it will be done in the server test. This will include the creating of a player object, and the storing and updating of values such as realName, gold, seenMap, etc. 

