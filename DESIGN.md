# CS50 Nuggets
## Design Spec
### Wall Street Djournal - CS50 23W

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes player and maps/visibility modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Client (Player / Spectator)

The *client* acts in one of two modes:

 1. *spectator*, the passive spectator mode described in the requirements spec.
 2. *player*, the interactive game-playing mode described in the requirements spec.

### User interface
 *a spectator can only quit the game*
* `Q` quit the game.
 * `h` move left, if possible
 * `l` move right, if possible
 * `j` move down, if possible
 * `k` move up	, if possible
 * `y` move diagonally up and left, if possible
 * `u` move diagonally up and right, if possible
 * `b` move diagonally down and left, if possible
 * `n` move diagonally down and right, if possible

### Inputs and outputs

**Inputs (from  requirments spec):**
 ```
 ./client hostname port [playername]
 ```
 To start the client, one must include one argument hostname port, and playername. The hostname is found from the output of the server.

**Outputs:**
 * error messages are logged to the file given in command line input
 * the game display to stdout

### Functional decomposition into modules

1. *main*, recieves input when client is called from the command line, calls *parseArgs*, and calls *handleInputs*
2. *parseArgs* parses the input given to main from the command line checking for at least 3 arguments, validating them, and marking whether the player is a spectator or not.
3. *handleInputs* watches for key board inputs from the user, sends them to the server watching, watches for new information from the server
4. *updateDisplay* outputs the correct status and game display to stdout
 
### Pseudo code for logic/algorithmic flow

**main:**

    initialize pointers to required inputs
    pass the pointers and argc and argv to handle args
    Initialize the display.
    Initialize the network and join the game with a PLAY or SPECTATE message depending on the output of parseArgs.
    call handle inputs

**parseArgs:**
    
    check if number of arguments is greater than 3
    validate the error log file as readable exit-non zero if not
    check if the there is a playername return 0 if the player is a spectator and 1 if not
    

**handleInputs:**
    
    while there is keyboard input
        send all keyboard inputs to the server
        if new information is recieved the server call update display
    send quit to the server and print game summary

**updateDisplay**
    
    get gameboard from server
    check if window size is large enough
        if so output the updated status line and gameboard to stdout
    

### Major data structures

No major data structures are needed for the client.

---

## Server
### User interface

See the requirements spec for the command-line interface.
There is no interaction with the user.

### Inputs and outputs

```
./server map.txt [seed]
```
To start the server, one must include one argument `map.txt`, and optional argument `seed`.
The `seed` argument creates a constant set of variables for the characterisitcs of the map such as location of players and location of gold.

### Functional decomposition into modules

 1. *server_input*, processes input from commandline
 2. *server_message*, sending and receiving messages
	a. *play*, When the message type is to play, initalize new player characteristics
	b. *spectate*, Allow spectator to view the game
	c. *quit*, Handle quitting message with table summer
	d. *key*, Handle multiple key inputs
 3. *server_dropGold*, drops random piles of gold with random counts
 4. *server_removePlayer*, processes player removal
 5. *server_visibility*, processing player visibility of different spots in a grid
 6. *server_nameTruncate*, shortens player name to allowed length
 8. *server_intialize*, announce the port number of the created network
 9. *server_gameOver*, print summary after ending game
 10. *server_playerMove*, this could be called from the key subsection of the server message to handle multiple cases if the new position is a another player, gold pile, or a blank spot.
 

The server will run as follows:

        parses the command line, validate parameters;
        calls server_dropGold to initialize gold;
        initialize game struct with information of player and gold;
        maintains master knowledge of player location and gold status, stored in game struct;
        initialize message loop with server_timeout, server_input, and server_message;


#### server_input:
        
        Processing input and validating the arguments
        
        
#### server_message:

	Parses message by message type;
	if message type is 'PLAY':
		Checks whether max player has been reached:
			if so, reply with 'QUIT' message;
		Checks validity of name:
			if name is valid:
				call name_truncate with name and allowed length;
				Decide letter for player;
				Reply with 'OK Letter','GRID' and 'GOLD' message;
				Stores processed name of player;
			if name is invalid:
				Responds with 'QUIT' message;
	if message type is 'SPECTATE':
		Reply with 'GRID' and 'GOLD' message'
		continue to update this spectator with complete display in loop
	if message type is 'QUIT':
		calls server_removePlayer
	if message type is 'KEY':
		validate whether key is allowed
		if key is allowed:
			update player location in struct
			update gold info if changed
			update the master display(for server and spectator to see)
			for all the player:
				Determine their visibility(call visibility)
				Reply with 'DISPLAY' message accordingly
			for all spectators:
				Reply with master 'DISPLAY' message
        

#### server_removePlayer:

    Takes a player name
    removes it from game struct
    remove symbol of player on map
        
#### server_visibility:

    Takes a grid, and a location in the grid;
    call grid_visible;
    call grid_display using output of grid_vision;
        
        
            
#### server_name_truncate:

    Takes a string and max length allowed
    Truncates the string based on length allowed
    Return truncated string


#### server_dropGold:

    drops at least GoldMinNumPiles and at most GoldMaxNumPiles of gold in the room using a random function

#### server_gameOver:

	Print out a summary of each player and the respecitve gold nuggets collected in a tabular form 

#### server_playerMove:

	Based on how the user moves, check what type of position (whether it's a empty spot, player or gold pile) it is and update grid accordingly.

### Major data structures

#### Game struct:
        * master: A grid struct variable to store all grid data
		* int seed: Optional seed entered to determine finite characterics of grid
        * player: player hastable where A-Z character as key, player data structure as item
        * int goldTotal: total gold that exists in the game
        * int goldNumPilesLeft: remaining gold left in the game
        * int goldCollected: number of gold collected in the game
        * int goldPileCount[x][y] = number of gold at that location
		* Spectator : New type to store spectator details

---

## Maps & Visibility
Maps will be a separate module that the server will access for its map data structure and methods.

The map structure will store the map as read from the map.txt file (there will be a method to load a map.txt file into a map), but it will not store the player or gold data (the server needs to do that). Instead, it will provide methods that take in the map struct pointer, a list of players, and a list of gold, and will return the ascii representation of the map with players and gold on it.

We'll design a number of methods to allow loading and setting the map, printing out the blank map, the map with players overlaid, and the map with gold overlaid, and the map with only some sections visible (for players not being able to see the whole map)

Visibility will work within the maps module. Given a player's position, we want to know if a certain gridpoint is visible. We'll use the method described in the requirements spec, where we look through the rows and columns between the player position (pr, pc) and the gridpoint we're trying to look at (r,c). If the line between these two points is ever between two NOT empty room spots, then the point will not be visible.

The server should store what gridpoints have been viewed by the player, and the maps module should have an isVisible() method that, for player position and gridpoint position, will return a bool value for if its visible. We can then iterate through the map and display only what's visible on the grid, and what has been previously seen.


### Functional decomposition

 1. *maps_read*, reads a map.txt file and loads it into a grid struct
 2. *maps_blankgrid*, converts the grid struct into a text representation of the map, without players or gold overlaid
 3. *maps_fullgrid*, converts the grid struct into a text representation of the map with all the players and gold overlaid
 4. *maps_playergrid*, converts the grid struct into a text representation of the map with only what a given player has seen, and only visible gold
 5. *maps_getDims*, returns the dimensions of the grid
 6. *maps_getVisiblePoints*, returns a list of points visible from a certain point
 7. *maps_getRandomGridpoint*, returns a random, valid (empty room) gridpoint


### Pseudo code for logic/algorithmic flow

#### maps_read

		open and validate the given maps.txt file
		go through text file and get dimensions of map
		initialize grid struct
		declare arrays of integers for grid struct
		go through text file and load characters into the grid struct

#### maps_blankgrid

		go through grid struct and return each character row separated by new lines

#### maps_fullgrid

	go through each grid struct with an empty string
		if the (x,y) is a player, add the player letter (A-Z) instead of room space to the string
		if the (x,y) is gold, add * instead of room space to the string
		otherwise spit out the char in the grid, rows separated by new line

#### maps_playergrid

	go through each grid struct with an empty string
		if the (x,y) is 
		if the (x,y) is a player, add the player letter (A-Z) instead of room space to the string
		if the (x,y) is gold, add * instead of room space to the string
		otherwise spit out the char in the grid, rows separated by new line

#### maps_getDims

	return number of rows and number of columns of grid

#### maps_getVisiblePoints

	go through each gridpoint in the grid (knowing the player position we're trying to look from)
		think of line between player position and test position
		if gridpoint is visible from player pos (if line is not between two empty room spots for each pair of gridpoints between the line through each row/column (or gridpoint if line lies directly on one))
			add to list of visible points
	return list of visible points

#### maps_getRandomGridpoint

	get positions of all empty room gridpoints
	pick a random one from that list
	return that one

### Major data structures

There will be a main data structure in the maps/visibility module will be the grid_t struct. This structure should hold the array of arrays of chars for each gridpoint in the map. We'll reserve the chars as listed in the requirements spec.
   * ` ` solid rock - interstitial space outside rooms
   * `-` a horizontal boundary
   * `|` a vertical boundary
   * `+` a corner boundary
   * `.` an empty room spot
   * `#` an empty passage spot

The grid_t* structure will also hold the integer number of rows and the number of columns for easy iteration boundaries.

---

## Player Module (data structure, different than client)

The player module will have the data structure that the server program will use to store and access information about a player.

The main data structure in the player module will be the player_t struct. This structure will contain either pointers to the stored values or if they are integers, the value of the integer. 

### Functional decomposition

1. *player_new*
2. *player_getGold*
3. *player_setGold*
4. *player_getVisibilityMap*
5. *player_setVisibilityMap*
6. *player_getXPosition*
7. *player_setXPosition*
8. *player_getYPosition*
9. *player_setYPosition*
10. *player_setPosition*
11. *player_getRealName*
12. *player_delete*

### Pseudo code for logic/algorithmic flow

Functions in the player module will be called by actions in the server module. Each player struct will be accessed by its corresponding alphabetical letter key. 

#### player_new 

> Allocate memory for and initialize a new player data 
> Set values for gold as 0, position at current position, name as given if less than maxNameLength (truncates and stores if not)

#### player_getGold
    
> Returns an integer representing the players current gold
    
#### player_setGold

> Updates the integer representing the players current gold

#### player_getVisibilityMap

> Returns an array of arrays with each element in the outer array representing a row in the map and each inner array representing a collumn in that array 
> The values stored in each inner array will either be 0 (if that coordinate in that map has not been seen) and 1 (if that coordinate in that map has been seen)

#### player_addVisibilityMap

> Adds either a 0 or a 1 to the inner array at the correct x (outer array) and correct y (inner array)

#### player_getXPosition

> Returns the players current x position on the map 

#### player_setXPosition

> Sets the players current x position on the map 

#### player_getYPosition

> Returns the players current y position on the map 

#### player_setYPosition

> Sets the players current y position on the map

#### player_getRealName

> Returns the players real name (truncated version less than given at the start of the game 

#### player_delete

> Frees individual components of the player data structure and frees that entire struct

### Major data structures

The player structure will store the (integer) amount of gold a player has, the position of the player, an array of arrays of the size of the map that masks where the player has been visible.

