# CS50 Nuggets
## Implementation Spec
### Wall Street Djournal - CS50 23W

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes x, y, z modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Plan for division of labor

> Update your plan for distributing the project work among your 3(4) team members.
> Who writes the client program, the server program, each module?
> Who is responsible for various aspects of testing, for documentation, etc?

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
typedef struct game {
  //Tracking gold left and collected
  int GoldNumPilesLeft;
  int goldCollected;             
  int goldLeft; 
  addr_t spectator; //Spectator address
  grid_t* mainGrid;  //Stores the m
  int numberOfRows;
  int numberOfColumns;
  player_t* players[MaxPlayers]; //array of player struct
  int numPlayer; 
  
} game_t;

typedef struct player {
  int row;                         // row
  int col;                         // column 
  addr_t IP;                       // IP address
  char realName[MaxNameLength + 1];
  char alias;                       // letter assigned
  int gold;                        // gold in purse
  int justCollected;
  grid_t* seenGrid;
} player_t;

```
### Definition of function prototypes

A function to parse the command-line arguments, initialize the game struct, initialize the message module, and create random behavior based on whether or not the seed phrase has been passed.

```c
static int parseArgs(const int argc, char* argv[]);
```

#### `parseArgs`:

	validate commandline
	verify map file can be opened for reading
	if seed provided based on no. of arguments
		verify it is a valid seed number
		seed the random-number generator with that seed
	else
		seed the random-number generator with getpid()
		
A function for sending and receiving messages a. play, When the message type is to play, initalize new player characteristics b. spectate, Allow spectator to view the game c. quit, Handle quitting message with table summer d. key, Handle multiple key inputs

```c
bool server_message(void* arg, const addr_t from, const char* message);
```

#### `server_message`:

	validate commandline
	verify map file can be opened for reading
	if seed provided based on no. of arguments
		verify it is a valid seed number
		seed the random-number generator with that seed
	else
		seed the random-number generator with getpid()

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
void initClient(client_t* client);
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

## XYZ module

```C
typedef struct map {
	char** grid;
	int numRows;
	int numCols;
} map_t;

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
void initClient(client_t* client);
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
`maps_playergrid` renders the map into the string to pass to the client with only the gridpoints seen by a given player and gold, and players visible to a given player.

	validate args
	create gold grid from gold list, a 2d array of integers with same dimensions as the map that has the amount of gold at that point (0 if no gold there)
		initialize empty 2d array of ints
		iterate through gold list
			set the int at the row,col of the gold to the amount of gold it is
	
	create player grid from player list, a 2d array of chars with the same dimensions as the map
		initialize the empty 2d array of chars
		iterate through player list
			set the char at the row,col of the player to the char [A-Z] representing the player 

	create remembered points grid from given player's list of 

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

## Testing plan

### unit testing

> How will you test each unit (module) before integrating them with a main program (client or server)?

### integration testing

> How will you test the complete main programs: the server, and for teams of 4, the client?

### system testing

> For teams of 4: How will you test your client and server together?

---

## Limitations

> Bulleted list of any limitations of your implementation.
> This section may not be relevant when you first write your Implementation Plan, but could be relevant after completing the implementation.
