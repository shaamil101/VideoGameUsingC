# CS50 Nuggets
## Implementation Spec
### Wall Street Djournal - CS50 23W

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes maps/visibility and player modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Plan for division of labor

Wyatt is writing maps / visibility
Shaamil is writing server program
Jake is writing player module
Jack is writing client program

## Server

### Data structures

We need one primary data structure for the entire game logic. The game structure has the following elements:

```c
typedef struct game
{
        // Instance variables
        char *playersChar; // holds the char the tabular summary
        int numplayers;
        int num_nuggets;
        spec_t *spectator;
        map_t *map;
        playerTable_t *players; // holds a player with the table
} game_t;
```

A data structure which holds the player, their address, and their letter 
typedef struct playerNode
```c
typedef struct playerNode
{
        addr_t address;
        char *playerLetter;
        player_t *player;
} playerNode_t;
```
A data structure which holds all of the players
```c
typedef struct playerTable
{
        playerNode_t *arr[MaxPlayers];
} playerTable_t;
```
A data structure which holds the one spectator

```c
typedef struct spectator
{
        addr_t address;
} spec_t;
```
Data structures that holds gold pile amount and location and number of gold piles

```c

typedef struct gold_pile
{
  int amount;
  int x;
  int y;
} gold_pile_t;

typedef struct gold
{
  gold_pile_t* piles;
  int num_piles;
} gold_t;
```
### Definition of function prototypes

```C
void server_dropGold(map_t *map, int num_piles, int gold_amount);
bool handleMessage(void *arg, const addr_t from, const char *message);
gold_pile_t *new_gold_pile(int size);
void delete_gold_pile(gold_pile_t *pile);
void playerTable_delete(playerTable_t *playerTable, int yRange);
spec_t *spectator_new(addr_t address);
playerTable_t *playerTable_new();
game_t *gamenode_new(int num_nuggets, map_t *curr_map);
void send_player_display(game_t *game, player_t *player, addr_t from);
void send_player_gold(game_t *game, player_t *player, addr_t from);
void send_spectator_gold(game_t *game, addr_t from);
char *createGoldMessage(map_t *map, player_t *player);
player_t *player_set(map_t *map, player_t *player);
char getCharacterBasedOnIndex(int i);
void move_left(map_t *map, player_t *player);
void move_right(map_t *map, player_t *player);
void move_up(map_t *map, player_t *player);
void move_down(map_t *map, player_t *player);
void move_diag_down_right(map_t *map, player_t *player);
void move_diag_down_left(map_t *map, player_t *player);
void move_diag_up_right(map_t *map, player_t *player);
void move_diag_up_left(map_t *map, player_t *player);
void move_diag_up_right_MAX(map_t *map, player_t *player);
void move_diag_up_left_MAX(map_t *map, player_t *player);
void move_diag_down_right_MAX(map_t *map, player_t *player);
void move_diag_down_left_MAX(map_t *map, player_t *player);
void move_up_MAX(map_t *map, player_t *player);
void move_right_MAX(map_t *map, player_t *player);
void move_left_MAX(map_t *map, player_t *player);
void move_down_MAX(map_t *map, player_t *player);
bool moveable(mapNode_t *node);
void server_dropPlayer(map_t *map, player_t *player);
void player_move(map_t *map, player_t *player, int new_x, int new_y);
char *game_over_summary();
player_t *searchByAddress(addr_t from);
bool isNumber(char number[]);
void free_everything(game_t *game);
void make_visible(player_t *player, map_t *map);
void send_spectator_display(game_t *game, addr_t from);
```

### Detailed Psuedocode


Main performs a series of checks and initializes the necessary components before starting the game. It validates the number of arguments passed to the program, initializes the logging file, and sets up the game environment based on the arguments provided. Finally, it starts the server and waits for clients to connect. Once the game is complete, it cleans up any resources used by the program and exits.

```c
int main(int argc, char *argv[])
```

#### `main`:

	The `main` function takes two arguments, `argc` and `argv`. 

	It first checks if the number of arguments is less than 2 or greater than 3. If the number of arguments is invalid, it prints an error message and returns 1. 

	If the number of arguments is valid, the function:
    - Initializes a variable `map_pathname` to the path of the map file, which is the second argument. 
    - Initializes a logging file 
    - Checks if a seed value has been provided as the third argument. 

    If a seed value is provided, it:
        - Checks that the seed is a positive integer 
        - Sets the seed for the random number generator. 

    Else
        - Sets the seed to the current process ID.

	Next, the function:
    - Loads the map from the file 
    - Drops gold nuggets on the map 
    - Initializes the game and the network

	The function then:
    - Determines a port number 
    - Announces the port number
    - Creates an internet address and initializes it in a call to `message_setAddr`
    - Enters a loop to handle messages using the `message_loop` function and the `handleMessage` function as the message handler.
    - Prints a thank-you message when the loop is exited.


A function called to drop random piles of gold in the map
```c
void server_dropGold(map_t *map, int num_piles, int gold_amount);
```

#### `server_dropGold`:

	Get X and Y ranges by calling the map module
	Count the amount of open spaces where gold can go
	Run a random function in the range of open spaces to determine gold location
	Check if the item at that coordinate is an empty spot
		Then switch the empty spot for a gold pile and call new gold pile
		Also set the map node type to be a gold type
	else
		Place into a new spot instead 
	Decrement the number of piles left and the open spaces available at the end
		
This function  is a utility function that allows a server to handle incoming messages from clients and take appropriate actions based on the contents of the message.

```c
bool handleMessage(void *arg, const addr_t from, const char *message)
```

#### `handleMessage`:

	Check if the arg parameter (which is expected to be an addr_t pointer) is NULL. If it is NULL, return true.
	Store the from parameter in the addr_t pointer otherp that was passed in as arg. This addr_t pointer is used to keep track of the current sender, so that future messages can be sent back to that sender.
	Check if the message starts with the string "PLAY ". If it does, continue with the following steps:
		Check if the number of players in the current game has already reached the maximum number of players allowed (MaxPlayers). If it has, print an error message to stderr and return false.
		Parse the player's name from the rest of the message by looking for a space character (' ') and splitting the message there. If no space character is found, print an error message to stderr and return false.
		Create a new player_t object with the given name and other parameters (like the player's starting position and appearance), and add it to the game's map.
		Create a new playerNode_t object to hold the new player and add it to the game's players array at the first available index.
		Assign a unique letter to the new player based on the current number of players in the game, and send a message back to the player indicating their assigned letter.
		Increment the number of players in the game.
		Send updated game information (like the map, gold, and display) to all players and spectators.
		Return false.
	Check if the message starts with the string "SPECTATE". If it does, continue with the following steps:
		Check if there is already a spectator in the game. If there is, send them a game over summary message and remove them from the game.
		Create a new spectator_t object with the sender's address and add it to the game's spectator field.
		Send updated game information (like the map, gold, and display) to the new spectator.
		Return false.
	Check if the message starts with the string "KEY Q". If it does, continue with the following steps:
		Set the game's quit field to true.
		Return true.
	If none of the above conditions match, return false.

The function creates a new gold pile with the specified amount of gold.

```c
gold_pile_t *new_gold_pile(int size);
```

#### `new_gold_pile`:

	Allocate memory for a new gold pile of type gold_pile_t and assign it to a pointer variable named pile
    if pile is NULL:
        return NULL
    else:
        set the amount of gold in the pile to size
        return pile

The function creates a new gold pile with the specified amount of gold.

```c
void send_player_display(game_t *game, player_t *player, addr_t from);
```

#### `send_player_display`:

	Start by creating a string that represents the game board.
	Determine the size of the message to be sent by adding the length of the game board string and the string "DISPLAY\n".
	Allocate memory for a new string to hold the display message.
	Copy the string "DISPLAY\n" to the beginning of the new display message string.
	Concatenate the game board string to the end of the display message string.
	Send the display message to the specified player address.
	Free the memory used for the game board string and display message string.

The function is to send a message containing information about the amount of gold left in the game to a spectator.

```c
void send_spectator_gold(game_t *game, addr_t from)
```

#### `send_spectator_gold`:

	Calculate the length of the message to be sent
	Allocate memory for the message string
	Determine the current gold count in the game
	Format the message string with the gold count
	Send the message to the spectator
	Free the memory allocated for the message string

This function is called when a new player is created or when a game is started to set the initial position of the player on the map and initialize the seen map with false values for all positions.
```c
player_t *player_set(map_t *map, player_t *player)
```

#### `player_set`:

	If map is null, then return null and log an error message
  	Set a variable called open_spaces to 0
  	Loop through the rows of the map and for each column:
    Check if the grid is not null
      Check if the character at the current grid position is a period
        If so, increment open_spaces counter by 1
  	Generate a random integer called player_location between 0 and the value of open_spaces
  	Set a variable called space to 0
 	 Loop through the rows of the map and for each column:
    Check if the grid is not null
      Check if the character at the current grid position is a period
        If so, check if space is equal to player_location
          If it is, set the player's X and Y positions to the current grid position
          If it is not, increment the space counter by 1
  	Loop through the rows of the map and for each column:
    Set the player's seen map for the current grid position to false
  	Return the player pointer

This function  is responsible for moving the player within the game map. It takes the current map, player object, and the new x and y coordinates to move the player to. The function checks for the type of the item present in the new position and performs different actions based on it. If it's a gold pile, it subtracts the gold from the map and adds it to the player's inventory. If it's a hallway, it simply switches the character with an empty space. If it's another player, it swaps the positions of the two players. Finally, the function updates the game map with the new positions of the players and the items

```c
void player_move(map_t *map, player_t *player, int new_x, int new_y)
```

#### `player_move`:

	Start by getting the player's current x and y positions.
	Determine the character and item types of the node at the new x and y position on the map.
	If the node contains a pile of gold, subtract the amount of gold from the total gold remaining on the map, add it to the player's gold count, set the character to an empty space, set the item type to null, and delete the gold pile.
	If the node contains a hallway, set the character to an empty space and the item type to null.
	If the node contains another player, swap the two players' positions, set the character to the character at the new position, and set the item type to the item type at the new position.
	Otherwise, set the character to the character at the new position, and set the item type to the item type at the new position.
	Set the player's x and y positions to the new position.
	Set the character and item type of the node at the player's previous position to the character and item type of the new position.
	Make the area around the player visible on the map.

This function creates a string containing a summary of the game results for all players, including their assigned letter, amount of gold, and real name. This function is called at the end of the game to display the summary to the players. The summary can be sent as a message to the players, displayed on a screen, or saved to a file for future reference. The returned string can be used in a variety of ways, depending on the implementation of the game.

```c
char *game_over_summary()
```

#### `game_over_summary`:

	Create a string variable to hold the game over summary message.
	Allocate memory for the message string.
	Add the "QUIT" command to the beginning of the message.
	Append a thank you message to the message string.
	Append a heading for the player summary table to the message string.
	Append a separator line to the message string.
	Create a temporary variable to hold the number of players in the game.
	Use a loop to iterate through all players and add their information to the summary table.
	Get the player's assigned letter, gold count, and real name.
	Convert the player's gold count to a string format.
	Append the player's information to the message string in the proper format.
	Decrement the number of players to iterate through and free any memory allocated for temporary variables.
	Print the message string to the console for verification.
	Return the message string.

This function is used to update a player's "seen map" by marking nodes as visible if they are within the player's line of sight. The function takes in a player and a map, and uses the player's current position and line of sight range to determine which nodes should be marked as visible. This function is typically called after a player moves or the game state changes, in order to update the player's view of the game world.

```c
void make_visible(player_t *player, map_t *map)
```

#### `make_visible`:

	Loop through all the nodes in the map.
	For each node, check if the player has not already seen it before.
	If the node is visible from the player's current position, mark it as seen on the player's seen map.
	Repeat for all nodes in the map.

This function is used to deallocate all the memory that was allocated during the game. When the game is over or the server is shutting down, it's important to free up all the memory that was allocated to avoid memory leaks. The free_everything() function does this by freeing all the memory that was allocated to the game object, including the map, the player table, and the spectator.
```c
void free_everything(game_t *game)
```

#### `make_visible`:

	Check if the game has a spectator. If yes, free the memory allocated to it.
	Delete the playerTable and all the players in it.
	Delete the map and all the nodes in it.
	Free the memory allocated to the game object.

This function determines whether a given node on a map is moveable or not. It takes a pointer to a map node as input and returns a boolean value indicating whether it is safe to move onto the node or not. The function checks if the node is not null and if it contains a valid character ('.', '*', '@', or '#') which denotes empty space, a box, the player, or a target respectively.

```c
bool moveable(mapNode_t *node)
```

#### `moveable`:

	Define a function called "moveable" that takes a single argument, a pointer to a mapNode_t object.
	Create a local variable called "this_node" and set it equal to the input argument.
	Create a character variable called "c".
	Check if "this_node" is not NULL.
	If "this_node" is not NULL, get the value of the mapNode item and assign it to "c".
	If "this_node" is NULL, return false.
	Check if "c" is equal to '.', '*', '@', or '#'.
	If "c" is equal to any of these characters, return true.
	If "c" is not equal to any of these characters, return false


A function that accepts a player and removes it by calling the remove player 

```c
bool handlePlay(void* arg, const addr_t from, const char* name)
```

#### `handlePlay`:

	if the game is full
        send a quit message back to the player
    if the name is Empty
        send a quit message back
    otherwise
        create a new player by calling the player module function
        copy the player name in to the real name
        initialize gold related variable
        drop the player on to the grid, update the player location and the master grid
        increment the number of players in the game
        send update to all clients

A function that accepts a player and removes it by calling the remove player 

```c
bool handleKEY(void* arg, const addr_t from, const char* key);
```

#### `handleKey`:

	if the key is Q
        call handleQUIT with the IP
        return flase to continue the loop
    otherwise
        depending on the key
            for lower case
                call player_move with new location
            for Upper case
                call player_move with new location until return true
    if no gold left
        return true
    else
        return false

A function that drops random piles of gold with random counts

```c
static void server_dropGold(void);
```

#### `server_dropGold`:

	generate a random number for the number of goldpiles
    for each pile
        generate random location update the mastergrid
        while that location is a empty room spot
            generate a new random location
        update the 2d integer array for the gold piles

A function that drops players in random locations

```c
static void server_dropPlayer(player_t* player);
```

#### `server_dropPlayer`:

	Generate random valid x location
	Generate random valid y location
	Set x location by calling player module
	Set y location by calling player module


A function that process player movement by checking the new positions the player wants to move and acting accordingly

```c
bool player_move(player_t* player, int new_row, int new_col);
```

#### `player_move`:

	if the new location is allowed to move to 
        if the new location is a player
            switch positions
            update grid
            send update to all clients
            return false
        if the new location is gold
            update the new location of the player
            update grid 
            call player add gold to pick up gold
            send update to all clients
        else (location is just empty room spot)
            update the new location of the player
            replace the old location with the original character
            update grid
            send update to all clients
            return false
    else
        return true

A function that process the function of creating a new spectator when needed and calling the message module
```c
bool handleSpectate(void* arg, const addr_t from);
```

#### `handleSpectate`:

	 if there exists one spectator
        send a quit message to the existing one
    store the spectator IP in to game struct (auto replace if needed)
    send a grid info message
    update all clients
    return false

A function that ends the game by printing a tabular form of results
```c
static void server_gameOver(void);
```

#### `server_gameOver`:

	Print out a summary of each player and the respecitve gold nuggets collected in a tabular form 

A function that ends the game by printing a tabular form of results
```c
static void server_update_all_clients(void);
```

#### `server_update_all_clients`:

	if there is a spectator
			send gold info 
			send the master grid
		iterate through every player
			send gold info 
			reset the just picked up gold to zero
			call set_visibility to get the visibilty grid and send
---

## Client

### Data structures

The is a client data stucture to hold the server address, player name, player id, and a boolean for whether it is a specator or not

```c
typedef struct client {
	addr_t serverAddress; 
	char playerId; 
	char* playerName; 
	bool isSpectator; 
} client_t; 
```

### Definition of function prototypes
```c
static client_t* parseArgs(const int argc, char* argv[]);
void startClient(client_t* client);
bool handleInputs(void* arg);
bool handleMessage(void* arg, const addr_t addr, const char* message);
void handleQuit(const char* message);
void handleGold(const char* message, client_t* client);
void handleGrid(const char* message);
void handleDisplay(const char* message);
void handleError(const char* message);
void handleInvalidMessage();
```

### Detailed pseudo code

#### `main`:

	allocate memory for a client data structure
	initialize pointers to required inputs
	pass the pointers and argc and argv to parseArgs
	call startClient
	call message_loop
	call message_done
	free client memory


#### `parseArgs`:
	allocate memory for client
	if there not exactly 3 or 4 arguments
  		log error to stderr
		exit non-zero
	if the port number is not retrievable using message_init
		log error to stderr
		exit non-zero
	if unsuccessfull set server address using message_setAddr
		log error to stderr
		exit non-zero
	if there are 3 args
		set player name to null and isSpectator to true
	if there are 4 args 
		set player name to fourth arg and isSpectator to false
	return the client


#### `startClient`:

	set up ncurses
	set up network
		if it is a player
  			send PLAY message to server
		if it is a spectator
  			send SPECTATE message
	draw the display


#### `handleInputs`:

	get the key inputted by the user
	if user is player
		send message using message_send
	refresh screen


#### `handleMessage`:

	check for the type of message
	handle the message using the corresponding function


#### `handleQuit`:

	prints a quit message to stderr


#### `handleGrid`:

	ensure that the size of the user's window is valid
	resize the window until it can fit the grid


#### `handleDisplay`:

	updates ncurses display based on message received by server


#### `handleGold`:

	print a gold status message
	print gold collected messages to playera
	print total nuggets left to spectators


#### `handleError`:

	prints an error message
	refresh

#### `handleInvalidMessage`:

	print error status message to display
	log error status message
	redraw display
	

---

## Maps / Visibility Module (named mapsandvis)

 The maps / visibility module provides a map data structure and a mapNode data structure and methods to load and validate maps and overlay with players and gold.
 
 The map struct will have a 2D-array of mapNodes, and have integers for how tall (number of rows) and wide (number of columns) it is. 
 
 The matrixNode structure holds info about the room character at that gridpoint, the type of point in the node (player, gold, base map), and booleans for whether or not it's transparent and a hallway.

### Data structures

```C
typedef struct mapNode{
  char item; // map char
  void* type; // type of item
  bool isTransparent;
  bool isHallway;
} mapNode_t;

typedef struct map{
	int numRows;
	int numCols;
  int totalGoldLeft;
  mapNode_t*** grid; //2d array of mapNode_t pointers
} map_t;
```

### Definition of function prototypes

```C
map_t* maps_new(char* mapTextAddress);
char* maps_basegrid(map_t* map);
char* maps_spectatorgrid(map_t* map);
char* maps_playergrid(map_t* map, player_t* player);
bool maps_isVisible(map_t* map, int playerX, int playerY, int testX, int testY);
int maps_getRows(map_t* map);
int maps_getCols(map_t* map);
int maps_getXrange(map_t* map);
int maps_getYrange(map_t* map);
mapNode_t* maps_getMapNode(map_t* map, int x, int y);
char maps_getMapNodeItem(mapNode_t* node);
void* maps_getMapNodeType(mapNode_t* node);
void maps_setMapNodeItem(mapNode_t* node, char item);
void maps_setMapNodeType(mapNode_t* node, void* type);
void maps_setTotalGoldLeft(map_t* map, int totalGoldLeft);
int maps_getTotalGoldLeft(map_t* map);
void maps_delete(map_t* map);
bool maps_isHallwayNode(mapNode_t* node);
static mapNode_t* mapNodeNew(char item);
static void mapNodeDelete(mapNode_t* node);
static bool firstAndEighthOctant(map_t* map, int playerRow, int playerCol, int testRow, int testCol);
static bool secondAndThirdOctant(map_t* map, int playerRow, int playerCol, int testRow, int testCol);
static bool fourthAndFifthOctant(map_t* map, int playerRow, int playerCol, int testRow, int testCol);
static bool sixthAndSeventhOctant(map_t* map, int playerRow, int playerCol, int testRow, int testCol);
```

### Detailed pseudo code

#### maps_new
`maps_new` takes in a mapTextAddress (the char* filepath to a map.txt file), allocates memory for a new map struct, verifies and then reads the file and parses it into a 2d array of characters. Memory allocated for this must later be freed with `maps_delete`. Psuedodode:

	verify mapTextAddress is a readable file (in requirements we are allowed to assume it is a valid map)
	allocate memory for a map struct, still unkown number of rows and columns
	read the file into one character buffer
	first pass just iterate through file string and count number of chars before first new line, and count number of new lines to get number of rows and number of columns
	set row and column numbers of map struct
	allocate memory for the 2d char array of the map struct, with number of rows and columns for matrix size
	go again through character buffer, keeping track of row and column indices
		easy to check right here if char is valid (' ' - | + . # are valid)
		add the char to the matrix under that row,col count
	return the map struct

#### maps_basegrid
`maps_basegrid` renders the map into the string without any gold or players on it

	validate args
	allocate memory for string holding grid + 1 for termianting null
	for each row r in the map (starting from 0)
		for each column c in the map (starting from 0)
			add char at that index of the map_t->grid 2d array to the string
		add a new line to the string (for next row)
	make last newline a terminating null instead
	return the string

#### maps_spectatorgrid

`maps_spectatorgrid` renders the map into the string to pass to the server client with all the gold and players on it

	validate args
	create gold grid from gold list, a 2d array of integers with same dimensions as the map that has the amount of gold at that point (0 if no gold there)
		initialize empty 2d array of ints
		iterate through gold list
			set the int at the row,col of the gold to the amount of gold it is
	
	create player grid from player list, a 2d array of chars with the same dimensions as the map
		initialize the empty 2d array of chars
		iterate through player list
			set the char at the row,col of the player to the char [A-Z] representing the player 


	allocate memory for string holding grid + 1 for terminating null
	for each row r in the map (starting from 0)
		for each column c in the map (starting from 0)
			if gold
				add '*' to the string
			else if player
				add player letter
			else
				add the normal map char at that index of the map_t->grid 2d array to the string
		add a new line to the string (for next row)
	make last newline a terminating null instead
	return the string

#### maps_playergrid
`maps_playergrid` renders the map into the string to pass to the client with only the map and a given player passed to it

	validate args (map and player can't be null)
	get number of rows, columns in map
	allocate memory for string holding grid + 1 for terminating null
	for each row r in the map (starting from 0)
		for each column c in the map (starting from 0)
			if bool in the seen grid is true
				if currently visible to player
					write *, A-Z, or other blocks
				else
					write base blocks (empty room or hallway for gold or player)
			else
				write empty space
		add a new line to the string (for next row)
	return the string

#### maps_isVisible
`maps_isVisible` returns a bool value for whether or not one point is visible from another point. It uses the method outlined in the CS50 nuggets assigment about taking the line between the two test points, and looking at the gridpoints the line intersects. If two of those gridpoints are 'opaque' (non-visible spaces), then the point isn't visible. Otherwise, it is. I'll define the player point as where we are looking from, and the test point as the point we're trying to view. I'll also define opaque characters as (' ' - | + #), i.e. anything but room space '.' 

For the octant cases, I'll just pass the map and playerX,Y and testX,Y points 

	set Xs to cols
	set Ys to rows
	validate args
		make sure both points row and column values are equal to or less than the map row and column (and non-negative)

	go through 13 cases:

	Goes through 13 cases:
  	same point as player
  	vertical line: up or down
  	horizontal line: left or right
 		perfectly diagonal line: up-right, up-left, down-right, down-left
 		sloped line: first and eigth octant (of coordinate quadrants), second and third, fourth and fifth, sixth and seventh octants

	same point as player:
		return true (visible to itself, sure)

	vertical down line case:
		get row difference between player and test point
		for each gridpoint along the row (player position incremented by (1 to testpoint]
			if gridpoint is opaque character (not room space '.')
			return false
		return true (we've made it to the end of the line segement)
	
	vertical up line case:
		get row difference between player and test point
		for each gridpoint along the row (player position decremented by (1 to testpoint]
			if gridpoint is opaque character (not room space '.')
			return false
		return true (we've made it to the end of the line segement)

	horizontal right line case:
		get column difference between player and test point
		for each gridpoint along the row (player position incremented by (1 to testpoint]
			if gridpoint is opaque character (not room space '.')
			return false
		return true (we've made it to the end of the line segement)

	horizontal left line case:
		get column difference between player and test point
		for each gridpoint along the row (player position decremented by (1 to testpoint]
			if gridpoint is opaque character (not room space '.')
			return false
		return true (we've made it to the end of the line segement)

	perfect diagonal cases:
		for each row/col between player and test point
			if gridpoint is opaque character
				return false
			increment/decrement each row/col depending on direction (four different directional cases)
		return true

	first and eighth octant:
		call firstAndEighthOctant with map pointer, and player row, col and test row, col points

	second and third octant:
		call secondAndThirdOctant with map pointer, and player row, col and test row, col points

	fourth and fifth octant:
		call fourthAndFifthOctant with map pointer, and player row, col and test row, col points

	sixth and seventh octant:
		call sixthAndSeventhOctant with map pointer, and player row, col and test row, col points


#### firstAndEighthOctant
first and eighth octants:
Args passed from isVisible, so we know they're good

		calculate row/col differences
		get the (double) slope of the line from player point to test point (change in rows divided by change in columns)
		get column difference 
		for each column between the player and test position (increment by 1 each time to go right)
			get the (double) row value of the line by doing (player_point + (change in column index * slope))
			if double is an integer:
				if gridpoint is opaque character
					return false
			else:
				with the integer row value rounded up and the integer row value rounded down,
				if both are an opaque character
					return false
		return true (we've made it to the end of the line segement)

#### secondAndThirdOctant
second and third octants:
Args passed from isVisible, so we know they're good
		
		calculate row/col differences
		get the (double) slope of the line from player point to test point (change in cols divided by change in rows)
		get row difference 
		for each row between the player and test position (decrement by 1 each time to go up)
			get the (double) col value of the line by doing (player_point + (change in row index * slope))
			if double is an integer:
				if gridpoint is opaque character
					return false
			else:
				with the integer col value rounded up and the integer col value rounded down,
				if both are an opaque character
					return false
		return true (we've made it to the end of the line segement)

#### fourthAndFifthOctant
fourth and fifth octants:
Args passed from isVisible, so we know they're good
		
		calculate row/col differences
		get the (double) slope of the line from the player point to test point
		get column difference
		for each column between the player and test position (decrement by 1 each time to go left)
			get the (double) row value of the line by doing (player_point + (change in column index * slope))
			if double is an integer:
				if gridpoint is opaque character
					return false
			else:
				with the integer row value rounded up and the integer row value rounded down,
				if both are an opaque character
					return false
		return true (we've made it to the end of the line segement)

#### sixthAndSeventhOctant
sixth and seventh octants:
Args passed from isVisible, so we know they're good
		
		calculate row/col differences
		get the (double) slope of the line from player point to test point (change in cols divided by change in rows)
		get cols difference 
		for each row between the player and test position (increment by 1 each time to go down)
			get the (double) col value of the line by doing (player_point + (change in row index * slope))
			if double is an integer:
				if gridpoint is opaque character
					return false
			else:
				with the integer col value rounded up and the integer col value rounded down,
				if both are an opaque character
					return false
		return true (we've made it to the end of the line segement)
					

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
`maps_delete` frees all the memory wrapped inside a map module, which is really just the 2d array and then the struct itself.

	validate args
	for each row in map grid
		for each col in map grid
			free node at map grid
		free row in map grid
	free the map grid
	free the map struct
---

## Player

 The player module provides a player data structure to store and update the values of the player. It is called by the server module throughout game play. 
 
### Data structures

The player data stucture holds the player name, seenMap, player gold, player justCollected (last amount of gold picked up), player IP address, player x and y position.

seenMap is a 2d array representing the grid points, and it stores booleans representing if the player has seen that point (true) or if they have not (false)

```c
typedef struct player{
    int x;                        
    int y;                    
    bool** seenMap;
 		addr_t IP;                     
    char realName[MaxNameLength];
    char letterAsisgned;                      
    int gold;                      
    int justCollected;
} player_t; 
```

### Definition of function prototypes

```c
player_t* player_new(const char* playerName, addr_t ipAddress, int maxCharacters, int totalRows, int totalCollumns, char letterAssigned);
void player_delete(player_t* player);
void player_addGold(player_t* player, int gold);
int player_getGold(player_t* player);
int player_getJustCollected(player_t* player);
void player_setJustCollected(player_t* player, int justCollected);
void player_addSeenMap(player_t* player, int row, int collumn);
bool** player_getSeenMap(player_t* player);
void player_setXPosition(player_t* player, int xPos);
int player_getXPosition(player_t* player);
void player_setYPosition(player_t* player, int yPos);
int player_getYPosition(player_t* player);
char* player_getRealName(player_t* player);
char player_getLetterAssigned(player_t* player);
void player_setLetterAssigned(player_t* player, char letter);
char* player_getIP(player_t* player);
```

### Detailed pseudo code

#### player_new

Allocates memory for a new player object, instantiates and allocates memory when necessary for values that player holds

	validate args 
		allocate space for new player object
		if player name is not less than max length 
			allocate space for name
			truncate length of name given to have no more than maxCharacters and store in name
			
		else
			allocate space for name
			set name to name given 
		set playername as given name
		set letter assigned as letter assigned given
		sets ipAddress as ipAddress given
		set gold to 0
		set just collected to 0
		allocate space for bool pointer seen map with the amount of rows (number of arrays in the outer array) and amount of collumns (number of elements in inner arrays). Setting all elements within map as false
		set x to 0
		set y to 0
		return player object
	return NULL

#### player_delete

frees the player object and the values stored within it 

	if player is valid
		free each array in the seen map
		free seen map 
		free realName 
		free player
	return

#### player_addGold

takes in an integer representing an amount of gold increments the players 
gold amount 

	if player is valid
		if gold is greater than 0
			get players gold 
			increment the gold by integer given 
			set players gold as new value 
	return 

#### player_getGold

returns an integer representing the players gold 

	if player is valid 
		get gold 
		return gold 
	return null

#### player_getJustCollected

Returns the players just Collected

	if player is valid
		return the players just collected value 
	return -1

#### player_setJustCollected

Aets the just collected value given an integer 

	if player does not equal null and just collected is greater than or equal to 0
		set player just collected as interger given 
	return 

#### player_addSeenMap

Adds either true or false to the inner array at the correct x (outer array) and correct y (inner array)

	if player is valid  
		index into the array with given row and collumn insert the boolean true indicating the spot is visible
	return 


#### player_getSeenMap

Returns an array of arrays storing either true or false for each point on the map indicating whether a player can see it or not 

	if player is valid
		get seen map 
		return seen map 
	return null

#### player_setXPosition
Sets the players current x position on the map. Server module checks if x is in range. 

	if player is valid 
		set players x value as value given 
return 

#### player_getXPosition
Returns the players current x position on the map 

	if player is valid
		get players x position 
		return x position 
	return -1

#### player_setYPosition
Sets the players current y position on the map. Server module checks if y is in range. 

	if player is valid 
		set players x value as value given 
	return 

#### player_getYPosition
Returns the players current y position on the map 

	if player is valid
		get players y position 
		return y position 
	return -1

#### player_getRealName

Returns the players real name (truncated version less than max length) given at the start of the game)

	if player is valid 
		get players real name 
		return real name 
	return null


### player_getLetterAssigned

Returns the players letter assigned (char)

	if player is valid 
		get and return the letter assigned 
	return null

### player_setLetterAssigned

 Sets the players letter assigned to the value given

	if player is valid and the letter is uppercase
		set the letter as the one given 
	return null

### player_getIP

Returns the players IP address

	if player is valid
		get players ip address
		return ip address
	return message no addr

---

## Testing plan

### unit testing

We can write a unit test for the maps module that will test load a map and test visibility and memory management methods, but the integration with the server will be tested while using the server.

The player module works so closely with the server that player module testing will happen in server program testing.

The server module runs with a bot as well as a seeded game with pre-determined steps. It can be found in the servertest.sh file

### integration and system testing
The server and client will be tested by using set seeds to allow for determined gold positioning and amounts. We can run with given client bots to test any behavior, and run the server on plank and client on our own machines to test client-server connections.

---

## Limitations
There are no known limitations or deviations from spec for our modules and programs, other than those outlined in the assignment details and requirements spec (such as a max of 26 players (through entire duration of server running) and 1 spectator (at a time) per server)
