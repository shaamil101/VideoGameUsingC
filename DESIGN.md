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

See the requirements spec for both the command-line and interactive UI.

> You may not need much more.

### Inputs and outputs

> Briefly describe the inputs (keystrokes) and outputs (display).
> If you write to log files, or log to stderr, describe that here.
> Command-line arguments are not 'input'.

### Functional decomposition into modules

> List and briefly describe any modules that comprise your client, other than the main module.
 
### Pseudo code for logic/algorithmic flow

> For each function write pseudocode indented by a tab, which in Markdown will cause it to be rendered in literal form (like a code block).
> Much easier than writing as a bulleted list!
> See the Server section for an example.

> Then briefly describe each of the major functions, perhaps with level-4 #### headers.

### Major data structures

> A language-independent description of the major data structure(s) in this program.
> Mention, but do not describe, any libcs50 data structures you plan to use.

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
        * player: player hastable where name as key, IP address as hashtable and his current location as item in an array
        * visibilityTable: player name as key, 2D array of grid parts he has seen as item
        * goldCounter: player letter (casted as integer) as key, gold collected as count
        * int goldTotal: total gold that exists in the game
        * int goldNumPilesLeft: remaining gold left in the game
        * int goldCollected: number of gold collected in the game
        * int goldPileCount[x][y] = number of gold at that location
		* Spectator : New type to store spectator details

---

## Maps & Visibility
Maps will be a separate module that the server will access for its map data structure and methods.
Quote on valid maps from requirements spec:


There will be a main data structure in the maps module will be the map_t struct. This structure should hold the array of arrays of chars for each gridpoint in the map. We'll reserve the chars as listed in the requirements spec.
   * ` ` solid rock - interstitial space outside rooms
   * `-` a horizontal boundary
   * `|` a vertical boundary
   * `+` a corner boundary
   * `.` an empty room spot
   * `#` an empty passage spot


The map structure will store the map as read from the map.txt file (there will be a method to load a map.txt file into a map), but it will not store the player or gold data (the server needs to do that). Instead, it will provide methods that take in the map struct pointer, a list of players, and a list of gold, and will return the ascii representation of the map with players and gold on it.

We'll design a number of methods to allow loading and setting the map, printing out the blank map, the map with players overlaid, and the map with gold overlaid, and the map with only some sections visible (for players not being able to see the whole map)

Visibility will work within the maps module. Given a player's position, we want to know if a certain gridpoint is visible. We'll use the method described in the requirements spec, where we look through the rows and columns between the player position (pr, pc) and the gridpoint we're trying to look at (r,c). If the line between these two points is ever between two NOT empty room spots, then the point will not be visible.

The server should store what gridpoints have been viewed by the player, and the maps module will have an isVisible() method that, for player position and gridpoint position, will return a bool value for if its visible. We can then iterate through the map and display only what's visible on the grid, and what has been previously seen.


### Functional decomposition

> List each of the main functions implemented by this module, with a phrase or sentence description of each.

### Pseudo code for logic/algorithmic flow

> For any non-trivial function, add a level-4 #### header and provide tab-indented pseudocode.
> This pseudocode should be independent of the programming language.

### Major data structures

> Describe each major data structure in this module: what information does it represent, how does it represent the data, and what are its members.
> This description should be independent of the programming language.

---

## Player Module

> Repeat this section for each module that is included in either the client or server.

### Functional decomposition

> List each of the main functions implemented by this module, with a phrase or sentence description of each.

### Pseudo code for logic/algorithmic flow

> For any non-trivial function, add a level-4 #### header and provide tab-indented pseudocode.
> This pseudocode should be independent of the programming language.

### Major data structures

> Describe each major data structure in this module: what information does it represent, how does it represent the data, and what are its members.
> This description should be independent of the programming language.
