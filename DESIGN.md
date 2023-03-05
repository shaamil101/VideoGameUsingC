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
 * error messages are logged to stderr
 * status messages above the display
 * the game display

### Functional decomposition into modules

1. *main*, recieves input when client is called from the command line, calls *parseArgs*, calls *startClient*, calls *message_loop*, calls *message_done*, and initializes a client data structure.
2. *parseArgs* parses the input given to main from the command line checking for at least 3 arguments, validating them, and marking whether the player is a spectator or not in the client object it returns if successful.
3. *startClient* sets nCurses up and initializes the network
4. *handleInputs* function used by message_loop for handling key inputs
5. *handleMessage* function used by message_loop for handling the messages
6. *handleQuit* handles the quit message
7. *handleGold* handles the gold message
8. *handleGrid* handles the grid message
9. *handleDisplay* handles the display message
10. *handleError* handles the error message
11. *handleInvalidMessage* handles an invalid message recieved from server
 
### Pseudo code for logic/algorithmic flow

**main:**

	allocate memory for a client data structure
	initialize pointers to required inputs
	pass the pointers and argc and argv to parseArgs
	call startClient
	call message_loop
	call message_done
	free client memory

**parseArgs:**
    
	initalize a client struct
    check if number of arguments is greater than or less than 3
	check if the port number is retrievable
	check setting the server address
	set client properties
	return the client
    
**handleInputs:**
    
    gets key inputs and sends message to server

**handleMessage**
    
	calls the correct handle method for the message recieved
    
**startClient**
    
    set up nCurses
	draw the screen
	build and send a key message from user input to the server
    

### Major data structures

There will be a client data structure that holds important information such as server address in one place

### Testing plan

We'll test various aspects of our client, including

1. arguments - ensure arguments are good to start client correctly
1. connection to server - handing bad addresses, bad ports, disconnection
1. window sizing - what happens when window is sized incorrectly or user never resizes
1. input handling - handling any button presses at any time
1. bad server messages - what happens when we receive bad data from the server
1. memory leaks - check for memory leaks while the program is running (exception being from ncurses library)

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

### Testing plan

We'll test our server program's various aspects, including:

1. bad arguments - make sure server start's running okay with correct arguments
1. connection to client - if multiple clients try to connect, multiple spectators, a client disconnects partway through the game
1. bad messages from client - handling all messages that the client could pass
1. memory leaks - check for memory leaks while the program is running

---

## Maps & Visibility
Maps will be a separate module that the server will access for its map data structure and methods.

The map structure will store the map as read from the map.txt file (there will be a method to load a map.txt file into a map), as a 2d array of mapNodes. It will provide methods that take in the map struct pointer and a player and return the ascii map representation.

We'll design a number of methods to allow loading and setting the map, printing out the spectator map and printing out specific client maps

Visibility will work within the maps module. Given a player's position, we want to know if a certain gridpoint is visible. We'll use the method described in the requirements spec, where we look through the rows and columns between the player position (pr, pc) and the gridpoint we're trying to look at (r,c). If the line between these two points is ever between two NOT empty room spots, then the point will not be visible.

The player module should store what gridpoints have been viewed by the player, and the maps module should have an isVisible() method that, for player position and gridpoint position, will return a bool value for if its visible. We can then iterate through the map and display only what's visible on the grid, and what has been previously seen.


### Functional decomposition

 1. *maps_new*, reads a map.txt file and loads it into a grid struct
 2. *maps_basegrid*, converts the grid struct into a text representation of the map, without players or gold overlaid
 3. *maps_fullgrid*, converts the grid struct into a text representation of the map with all the players and gold overlaid
 4. *maps_playergrid*, converts the grid struct into a text representation of the map with only what a given player has seen, and only visible gold
 5. *maps_getRows*, returns the number of rows of the grid
 1. *maps_getCols*, returns the number of columns of the grid
 1. *maps_getGridpoint*, returns the char gridpoint at a given row and column
 1. *isVisible*, private that returns whether or not a point is visble from another point on the base map
 6. *maps_getVisiblePoints*, returns a list of points visible from a certain point
 7. *maps_getRandomGridpoint*, returns a random, valid (empty room) gridpoint
 1. *maps_delete* deletes the map
 1. *maps_newMatrixIndex* creates a new matrix index structure that holds a row and column index
 1. *maps_deleteMatrixIndex* deletes the matrix index


### Pseudo code for logic/algorithmic flow

#### maps_new

		open and validate the given maps.txt file
		go through text file and get dimensions of map
		initialize grid struct
		declare arrays of mapNodes for grid struct
		go through text file and load characters into the mapNode

#### maps_basegrid

		go through grid struct and return each character row separated by new lines

#### maps_spectatorgrid

	go through each grid struct with an empty string
		if the (x,y) is a player, add the player letter (A-Z) instead of room space to the string
		if the (x,y) is gold, add * instead of room space to the string
		otherwise spit out the char in the grid, rows separated by new line

#### maps_playergrid

	go through each grid struct with an empty string
		if the (x,y) has been seen by the player
			if the (x,y) is currently visible to the player
				if the (x,y) is a player, add the player letter (A-Z) instead of room space to the string
				if the (x,y) is gold, add * instead of room space to the string
				otherwise spit out the char in the grid, rows separated by new line
			if not currently visible
				print out normal char in grid
		if not seen by player
			print out space into string

#### maps_isVisible

	make a line between player point and test point
	get where line intersects different points
	start from player point and increment towards test point
		if line interescts exactly on a point
			if point is not transparent
				return false
		wherever line intersects pair of columns (or pair of rows, depending on line slope)
			if both points are not transparent
				return false
	return true if it makes it to test point without returning false

#### maps_getRows

	Takes in a map pointer and returns the num of rows stored

#### maps_getCols

	Takes in a map pointer and returns the num of cols stored

#### maps_getXrange

	Takes in a map pointer and returns the max x value (exclusive) in the grid (which is number of columns)

#### maps_getYrange

	Takes in a map pointer and returns the max y value (exclusive) in the grid (which is number of rows)

#### maps_getMapNode

	Takes in a map pointer and x and y position and gets the map node at that x and y (converts from x and y to rows and columns)

#### maps_getMapNodeItem

	Takes in a map node pointer and returns the char item for that map node

#### maps_getMapNodeType

	Takes in a map node pointer and returns the void* type for that map node

#### maps_setMapNodeItem

	Takes in a map node pointer and char item and sets the char item for that map node

#### maps_setMapNodeType

	Takes in a map node pointer and void* type and sets the void* type for that map node

#### maps_setTotalGoldLeft

	Takes in a map pointer and integer totalgold left and sets the integer totalgoldleft for the map struct

#### maps_getTotalGoldLeft

	Take in a map pointer and return integer value for totalGoldLeft in map struct

#### maps_ishallwayNode

	Returns the bool for whether or not a node is a hallway, stored in node struct

#### maps_delete

	Frees all the memory wrapped inside a map module, which is really just the 2d array and then the struct itself.

### Major data structures
There will be a mapNode_t data structure that stores, in place of a gridpoint, a character for map representation, the type (player or gold or null) at that point, and booleans for transparency and if it's a hallway.

There will be a main map_t data structure in the maps/visibility module. This structure should hold the array of arrays of mapNode_t's for each gridpoint in the map. We'll reserve the chars as listed in the requirements spec.
   * ` ` solid rock - interstitial space outside rooms
   * `-` a horizontal boundary
   * `|` a vertical boundary
   * `+` a corner boundary
   * `.` an empty room spot
   * `#` an empty passage spot

The map_t structure will also hold the integer number of rows and the number of columns for easy iteration boundaries, and the amount of gold left

### Maps & Visibility Testing Plan
We'll test the mapsandvis module independtly with a small `mapstest` script (written in C) that will just try to load a map, display the base grid string and the the grid as visible from a single point, and free the map from memory. Anything having to do with displaying players and gold will be tested in the systems and integration test with the server (including trying to load a map that doesn't exist, which results in a NULL map_t pointer being returned to the server, which handles it by throwing an error and exiting non-zero).

---

## Player Module (data structure, different than client)

The player module will have the data structure that the server program will use to store and access information about a player.

The main data structure in the player module will be the player_t struct. This structure will contain either pointers to the stored values or if they are integers, the value of the integer. 

### Functional decomposition

1. *player_new*
2. *player_delete*
3. *player_getGold*
4. *player_setGold*
5. *player_getJustCollected*
6. *player_setJustCollected*
7. *player_getSeenMap*
8. *player_setSeenMap*
9. *player_getXPosition*
10. *player_setXPosition*
11. *player_getYPosition*
12. *player_setYPosition*
13. *player_getRealName*
14. *player_getLetterAssigned*
15. *player_setLetterAssigned*
16. *player_getIP*

### Pseudo code for logic/algorithmic flow

Functions in the player module will be called by actions in the server module. Each player struct will be accessed by its corresponding alphabetical letter key. 

#### player_new 

> Allocate memory for and initialize a new player data 
> Set values for gold as 0, position at current position, name as given if less than maxNameLength (truncates and stores if not)

#### player_delete

> Frees individual components of the player data structure and frees that entire struct

#### player_getGold
    
> Returns an integer representing the players current gold

#### player_addGold

> Updates the integer representing the players current gold

#### player_getJustCollected

> returns the players just collected value 

#### player_setJustCollected

> returns the players just collected value

#### player_getSeenMap

> Returns an array of arrays with each element in the outer array representing a row in the map and each inner array representing a collumn in that array 
> The values stored in each inner array will either be false (if that coordinate in that map has not been seen) and true (if that coordinate in that map has been seen)

#### player_addSeenMap

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

> Returns the players real name (truncated version less than max length) given at the start of the game)

### player_getLetterAssigned

> Returns the players letter assigned (char)

### player_setLetterAssigned

> Sets the players letter assigned to the value given

#### player_getIP

> Returns the players IP address


### Major data structures

The player structure will store the players gold, name, seen map, x and y, just collected, letter assigned that are accessed by the server. 

### Player testing plan

Becuase the player module really just stores info for the server to use, we won't include a unit test but will instead wrap the player testing into the server testing, including testing the server for memory leaks caused by the player module. The player module is exclusively used by the server, so any bad use-cases will be done by the server, which we can test by running the server.