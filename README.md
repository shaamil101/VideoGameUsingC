# CS 50: Nuggets Game
## Authors: Wyatt Ellison, Shaamil Karim Shaw Alem, Jake Lotreck, Jack Desmond
## GitHub usernames: wyattellison, shaamil101, JLotreck, jcdeesmond23

This repository contains the code for the CS50 "Nuggets" game, in which players explore a set of rooms and passageways in search of gold nuggets.
The rooms and passages are defined by a *map* loaded by the server at the start of the game.
The gold nuggets are randomly distributed in *piles* within the rooms.
Up to 26 players, and one spectator, may play a given game.
Each player is randomly dropped into a room when joining the game.
Players move about, collecting nuggets when they move onto a pile.
When all gold nuggets are collected, the game ends and a summary is printed.

## Materials provided

See the [support library](support/README.md) for some useful modules.

See the [maps](maps/README.md) for some draft maps.

## Modules

client: module used to interpret messages form sever and updates user interface so that user can interact to play the name 

server: used to handle all game logic, maintain game state displays, and displays game state to a player or spectator 

mapsandvis: provides a data stucture for storing the map that the sever interacts with and player and gold locations holds methods for acessing data structure and determining the visibility between two points. 

player: module used by server that creates a player object to store a players location, gold, seen map, letter, name, etc. The methods are called by server throughout


## Usage

* To build the modules, run `make`.
* To clean , run `make clean`.

client called with: `./client hostname port [playername]`

server called with: `./server map.txt [seed]`

## Assumptions & Comments 
There are no extra assumptions besides those laid out in the specs. See the specs for all details.

## Tests

Our main test is a systems test with the server, client, maps, and player all working together. We can test the server + player + maps with a bot client putting random inputs. The server and client will be tested by calling them in the command line with all possible arguments, including

  1. no arguments
  1. incorrect (too few and too many) arguments
  1. (server) non-existing map file (in assignment details we are allowed to assume a map is valid if it is getting passed to server)
  1. (server) w/ and w/o seed, and bad seed
  1. (client) bad hostname
  1. (client) bad port
  1. (client) larger than max character playername
  1. (client) join as spectator with a spectator already connected
  1. (client) join as player with max number of players already reached

  The map will mostly be tested in the systems test, but there is a small mapstest file in the mapsandvis module that, when run, will load a map and display it in the terminal. This just tests that the map can be read and printed out correctly, but does not test server nor player module interaction (this is done in systems test).
