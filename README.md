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
