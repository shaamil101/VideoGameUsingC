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

## Player

> Teams of 3 students should delete this section.

### Data structures

> For each new data structure, describe it briefly and provide a code block listing the `struct` definition(s).
> No need to provide `struct` for existing CS50 data structures like `hashtable`.

### Definition of function prototypes

> For function, provide a brief description and then a code block with its function prototype.
> For example:

A function to parse the command-line arguments, initialize the game struct, initialize the message module, and (BEYOND SPEC) initialize analytics module.

```c
static int parseArgs(const int argc, char* argv[]);
```
### Detailed pseudo code

> For each function write pseudocode indented by a tab, which in Markdown will cause it to be rendered in literal form (like a code block).
> Much easier than writing as a bulleted list!
> For example:

#### `parseArgs`:

	validate commandline
	initialize message module
	print assigned port number
	decide whether spectator or player

---

## Server

### Data structures

We need two primary data structures. The first will be for the entire game and another player structure with the player attributes. The first structure is the game structure with the following elements:

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
static int parseArgs(const int argc, char* argv[]);
void startClient(client_t* client);
bool handleInputs(void* arg);
bool handleMessage(void* arg, const addr_t addr, const char* message);
void handleQuit(const char* message);
void handleGold(const char* message, client_t* client);
void handleGrid(const char* message);
void handleDisplay(const char* message);
void handleError(const char* message);
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

	if there not exactly 2 or 3 arguments
  		print error to stderr
		exit non-zero
	if the port number is not retrievable using message_init
		print error to stderr
		exit non-zero
	if unsuccessfull set server address using message_setAddr
		print error to stderr
		exit non-zero
	else return a 1 if there is a player name and 0 if not


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

	updates string based on message received by server


#### `handleGold`:

	print a gold status message
	print gold collected messages to playera
	print total nuggets left to spectators


#### `handleError`:

	prints an error message
	refresh

---

## Maps / Visibility Module (named maps)

The maps / visibility module provides a map data structure and a matrixIndex data structure and methods to load and validate maps and overlay with players and gold. The map will be stored as a 2D-array of characters, and have integers for how tall (number of rows) and wide (number of columns) it is. The matrixIndex structure just wraps two integers for a row and a column index into a 2D array.

### Data 

```C
typedef struct map {
	char** grid;
	int numRows;
	int numCols;
} map_t;

typedef struct matrixIndex {
	int row;
	int col;
} matrixIndex_t;
```

### Definition of function prototypes

```C
map_t* maps_new(char* mapTextAddress);
char* maps_basegrid(map_t* map);
char* maps_fullgrid(map_t* map, PLAYER_T* playerList, GOLD_T* goldList);
char* maps_playergrid(map_t* map, PLAYER_T* player, GOLD_T* goldList);
int maps_getRows(map_t* map);
int maps_getCols(map_t* map);
char maps_getGridpoint(map_t* map, int row, int col);
static bool isVisible(map_t* map, matrixIndex_t* playerPosition, matrixIndex_t* testPosition);
matrixIndex_t* maps_getVisiblePoints(map_t* map, PLAYER_T* player);
matrixIndex_t* maps_getRandomGridpoint(map_t* map);
void maps_delete(map_t* map);
matrixIndex_t* maps_newMatrixIndex(int row, int col);
bool compare_matrixIndex(matrixIndex_t* indexA, matrixIndex_t* indexB);
void maps_deleteMatrixIndex(matrixIndex_t* index);
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
`maps_blasegrid` renders the map into the string to pass to the client without any gold or players on it

	validate args
	allocate memory for string holding grid
	for each row r in the map (starting from 0)
		for each column c in the map (starting from 0)
			add char at that index of the map_t->grid 2d array to the string
		add a new line to the string (for next row)
	make last newline a terminating null instead
	return the string

#### maps_fullgrid

`maps_fullgrid` renders the map into the string to pass to the client with all the gold and players on it

	validate args
	create gold grid from gold list, a 2d array of integers with same dimensions as the map that has the amount of gold at that point (0 if no gold there)
		initialize empty 2d array of ints
		iterate through gold list
			set the int at the row,col of the gold to the amount of gold it is
	
	create player grid from player list, a 2d array of chars with the same dimensions as the map
		initialize the empty 2d array of chars
		iterate through player list
			set the char at the row,col of the player to the char [A-Z] representing the player 


	allocate memory for string holding grid
	for each row r in the map (starting from 0)
		for each column c in the map (starting from 0)
			if int at the gold grid is not 0
				add '*' to the string
			if char at the player grid is not NULL
				add the player char to the string
			else: add the normal map char at that index of the map_t->grid 2d array to the string
		add a new line to the string (for next row)
	make last newline a terminating null instead
	return the string

#### maps_playergrid
`maps_playergrid` renders the map into the string to pass to the client with only the gridpoints, gold, and players visible to a given player.

	validate args
	create gold grid from gold list, a 2d array of integers with same dimensions as the map that has the amount of gold at that point (0 if no gold there)
		initialize empty 2d array of ints
		iterate through gold list
			set the int at the row,col of the gold to the amount of gold it is
	
	create player grid from player list, a 2d array of chars with the same dimensions as the map
		initialize the empty 2d array of chars
		iterate through player list
			set the char at the row,col of the player to the char [A-Z] representing the player 

	create visibility grid from given player's visible points, 2d array of bools with same dims as map
		initialize the empty 2d array of bools to all false
		iterate through the visible points list,
			set the bool at the row,col of the visible point to true

	allocate memory for string holding grid
	for each row r in the map (starting from 0)
		for each column c in the map (starting from 0)
			if bool in the visibility grid is true
				if int at the gold grid is not 0
					add '*' to the string
				if char at the player grid is not NULL
					add the player char to the string
				else: add the normal map char at that index of the map_t->grid 2d array to the string
			else
				add ' ' to the string
		add a new line to the string (for next row)
	make last newline a terminating null instead
	return the string

#### maps_getRows
`maps_getRows` returns the int number of rows in a map struct

	validate args
	return numRows int of given map struct

#### maps_getCols
`maps_getCols` returns the int number of cols in a map struct

	validate args
	return numCols int of given map struct
	
#### maps_getGridpoint
Returns the char at the row, column index of a given map grid

	validate args (valid map, non-negative row and column)
	return char at that index

#### isVisible
`isVisible` is a private-to-the-module (static) method that returns a bool value for whether or not one point is visible from another point. It uses the method outlined in the CS50 nuggets assigment about taking the line between the two test points, and looking at the gridpoints the line intersects. If two of those gridpoints are 'opaque' (non-visible spaces), then the point isn't visible. Otherwise, it is. I'll define the player point as where we are looking from, and the test point as the point we're trying to view. I'll also define opaque characters as (' ' - | + #), i.e. anything but room space '.' 

	validate args
		make sure both points row and column values are equal to or less than the map row and column (and non-negative)
	switch through 3 cases: vertical down line (change in columns is 0, change in rows is positive), vertical up line (change in columsn is 0, change in rows is negative), horizontal right line (change in rows is 0, change in columns is positive), horizontal left line (change in rows is 0, change in columns is negative), sloped right line (non-zero change in rows and columns, but change in columns is positive), sloped left line case (non-zero change in rows and columns, but change in columns is negative)
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

	sloped right line case:
		get the (float) slope of the line from player point to test point (change in rows divided by change in columns)
		get column difference 
		for each column between the player and test position (increment by 1 each time to go right)
			get the (float) row value of the line by doing (player_point + (change in column index * slope))
			if float is an integer:
				if gridpoint is opaque character
					return false
			else:
				with the integer row value rounded up and the integer row value rounded down,
				if both are an opaque character
					return false
		return true (we've made it to the end of the line segement)

	sloped left line case:
		get the (float) slope of the line from the player point to test point
		get column difference
		for each column between the player and test position (decrement by 1 each time to go left)
			get the (float) row value of the line by doing (player_point + (change in column index * slope))
			if float is an integer:
				if gridpoint is opaque character
					return false
			else:
				with the integer row value rounded up and the integer row value rounded down,
				if both are an opaque character
					return false
		return true (we've made it to the end of the line segement)
					

#### maps_getVisiblePoints
`maps_getVisiblePoints` returns a list (array) of visible points for a base map for a player's position, using the static isVisible method. Caller must later free array.

	allocate memory for gridpoint array, of size num_rows*num_cols (max number of visible points)
	keep track of index adding to array (start at 0)
	for each row r in map
		for each col c in map
			if isVisible(player row, player col, r, c)
				add gridpoint array
				increment array index
	return array

#### maps_getRandomGridpoint
returns a random gridpoint from a map, using the rand() method. **Random module must be first initialized by the server seed with srand(seed or getPID).** The rand() method's integer range is 0 to 2147483647 (from stdlib.h), so if we take one of those random numbers moduloed with the number of rows and number of columns (which we can assume for user playability will be much less than that max int), then it will be a pretty random value. That won't hold up if we get modulus values that get near the magnitude of the max int, but we won't get there with the size maps we're dealing with.

	initialize new matrixIndex struct
	set row of 2d index to (rand() % map->numRows)
	set col of 2d index to (rand() % map->numCols)
	return matrixIndex struct (must later be freed)

#### maps_delete
`maps_delete` frees all the memory wrapped inside a map module, which is really just the 2d array and then the struct itself.

	free the 2d array grid in the map struct
	free the map struct

#### maps_newMatrixIndex
Allocates memory for and initializes a new matrix index struct

	validate arg (no negative numbers, return null if so)
	assert / allocate memory for a matrixIndex struct
	set the row to the given row
	set the col to the given col
	return pointer to the struct

#### maps_deleteMatrixIndex
free the memory for a matrix index struct

	call free() on the struct if not null
---

## Player

### Data structures

The player data stucture to hold the player name, player gold, player visibility map, player x and y position.

```c
typedef struct player{
    int x;                        
    int y;                    
    grid_t* seenGrid;
 	addr_t IP;                     
    char realName[MaxNameLength];
    char letterAsisgned;                      
    int gold;                      
    int justCollected;player_t; 
} player_t; 
```

### Definition of function prototypes

```c
player_t* player_new(const char* playerName, addr_t ipAddress, int maxCharacters, int totalRows, int totalCollumns, char letterAssigned);
void player_delete(player_t* player);
void player_addGold(player_t* player, int* gold);
int player_getGold(player_t* player);
void player_addSeenMap(player_t* player, int* row, int* collumn);
bool** player_getSeenMap(player_t* player);
void player_setXPosition(player_t* player, int* xPos);
int player_getXPosition(player_t* player);
void player_setYPosition(player_t* player, int* yPos);
int player_getYPosition(player_t* player);
char* player_getIP(player_t* player);
```

### Detailed pseudo code

#### player_new

Allocates memory for a new player object, instantiates values for gold, creates visibility map, gets random location for player, stores the players name (truncates if necessary), sets the ipAddress as the address given

	validate args 
	allocate space for new player object
	if player name is not less than max length 
		truncate length of name given to have no more than maxCharacters
		set name to truncated name
	set name as given name
	allocate memory for real name
	set realName as name
	sets ipAddress as ipAddress given
	set gold to 0
	allocate space for visibility map with the amount of rows (number of arrays in the outer array) and amount of collumns (number of elements in inner arrays)
	if map is instantiated
		get random gridpoint from the map 
		set x to collumn from gridpoint
		set y to row from gridpoint
	return player object

#### player_delete

frees the player object and the values stored within it 

	if player is valid
		free visability map
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
	return null

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
	return null

#### player_getRealName

Returns the players real name (truncated version less than max length) given at the start of the game)

	if player is valid 
		get players real name 
		return real name 
	return null

### player_getIP

Returns the players IP address

	if player is valid
		get players ip address
		return ip address
	return null

---

## Testing plan

### unit testing

We can write a unit test for the maps module that will test load a few maps and test visibility methods, but the integration with the server will be tested while using the server.

The player module works so closely with the server that player module testing will happen in server program testing.


### integration testing

The server will be tested by having a dummy client send pre-determined messages testing all messages (including invalid ones and edge cases, and testing all player movement behavior, including normal behavior and erratic behavior.

The client will be tested by having a dummy server sending pre-determined messages to the client, testing all communcations (including invalid ones), and sending different map strings and other server messages, including edge cases for messages.

### system testing

The server and client will be tested by using set seeds to allow for determined gold positioning and amounts. We can run with given client bots to test any behavior, and run the server on plank and client on our own machines to test client-server connections.

---

## Limitations

-
-
