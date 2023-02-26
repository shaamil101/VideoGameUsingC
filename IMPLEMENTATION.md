# CS50 Nuggets
## Implementation Spec
### Team name, term, year

> This **template** includes some gray text meant to explain how to use the template; delete all of them in your document!

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
	verify map file can be opened for reading
	if seed provided
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

> For each module, repeat the same framework above.

### Data structures

### Definition of function prototypes

### Detailed pseudo code

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
