/* 
 * client.c - The client allows a human to join a nuggets
 * game as an interactive player or as a spectator. If a player name
 * is given the human joins as a player. Otherwise, they join as a
 * spectator
 * 
 * Usage: ./client hostname port [playerName]
 * 
 * Jack Desmond - February 25th, 2023
 * CS50 Winter 2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>
#include "../support/message.h"
#include "../support/log.h"
#include "../libcs50/mem.h"

/**************** global types ****************/
typedef struct client {
	addr_t serverAddress; 
	char playerId; 
	char* playerName; 
	bool isSpectator; 
} client_t; 

/**************** global variables ****************/
static const int maxStatusMessageLength = 50;

/**************** function headers ****************/
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

/**************** main() ****************/
/*
 * The main function for client.c (gets called when the client is called in the command line)
 * 
 * Input: hostname, port, playerName (optional)
 *
 * Output: 1 if successful and 0 if error
*/
int main(const int argc, char* argv[]) {
	// start the log module
	log_init(stderr);
	// parse the arguments
	client_t* client = parseArgs(argc, argv);
	// start the client
	startClient(client);
	// loop and listen for user key strokes
	message_loop(client, 0, NULL, handleInputs, handleMessage);
	message_done();
	// free the client
	mem_free(client);
	return 0;
}

/**************** parseArgs() ****************/
/*
 * The parseArgs function parses the input given 
 * to main from the command line checking for at least 
 * 3 arguments, validating them, and marking whether 
 * the player is a spectator or not.
 * 
 * Input: arguments given to main
 *
 * Output:
*/
static client_t* parseArgs(const int argc, char* argv[]) {
	// allocate memory for a client object
	client_t* client = mem_assert(mem_malloc(sizeof(client_t)), "allocating memory for client");
	
	// if incorrect number of arguments given log and return
	if (argc < 3 || argc > 4) {
		log_s("%s: Usage: ./client hostName port [playerName]\n", argv[0]);
		exit(1);
	}
	// if port number can't be fetched log and return
	int clientPort = message_init(NULL);
	if (clientPort == 0) {
		log_s("%s: Error: not able to fetch client port number\n", argv[0]);
		exit(1);
	}
	// if server address can't be set log and return
	if (!message_setAddr(argv[1], argv[2], &(client->serverAddress))) { 
		log_s("%s: Error: failed to set server address\n", argv[0]);
		exit(1);
	}
	// if the are only 3 args client is a spectator and client object is updated
	if (argc == 3) {
		client->isSpectator = true;
		client->playerName = NULL;
	// if there are 4 args given then client is a player and client object is updated
	} else if (argc == 4) {
		client->isSpectator = false;
		client->playerName = argv[3];
	}
	// return client object with correct information
	return client;
}

/**************** startClient() ****************/
/*
 * The startClient function sets nCurses up and initializes the network
 * 
 * Input: The client object initialized in main
 *
 * Output: None
*/
void startClient(client_t* client) {
	// set up nCurses
	initscr();
	cbreak();
	noecho();
	start_color();
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	keypad(stdscr, false);
	attron(COLOR_PAIR(1));

	char* serverMessage;

	// if the user is a spectator send a SPECTATE message to server
	if (client->isSpectator) {
		serverMessage = mem_assert(mem_malloc(strlen("SPECTATE") + 1), "allocating server message memory");

		strcpy(serverMessage, "SPECTATE");
		message_send(client->serverAddress, serverMessage);

	// if the user is a player send a PLAY message to the server
	} else {
		serverMessage = mem_assert(mem_malloc(strlen("PLAY ") + strlen(client->playerName) + 1), "allocating server message memory");

		sprintf(serverMessage, "PLAY %s", client->playerName);
		message_send(client->serverAddress, serverMessage);
	}

	// free message memory
	mem_free(serverMessage);
	// refresh the display
	refresh();
}

/**************** handleInputs() ****************/
/*
 * handleInputs is passed to the message_loop function
 * and it handles key press inputs from the user
 * 
 * Input: a client object
 *
 * Output: boolean false if message_loop should continue and
 * true if it shouldn't
*/
bool handleInputs(void* arg) {
	client_t* client = arg;

	// gets the pressed by player and initializes the keyMessage string
	int keyPressed = getch();
	char keyMessage[strlen("KEY " + 2)];
	
	// build the keyMessage to be sent to the server
	sprintf(keyMessage, "KEY %c", keyPressed); 

	// if the client is not a spectator or Q is pressed send it to the server
	if (!client->isSpectator || keyPressed == 81) {
		message_send(client->serverAddress, keyMessage);
	}

	// refresh the display
	refresh();

	return false;

}

/**************** handleMessage() ****************/
/*
 * The handleMessage function is passed to message_loop
 * and handles messages recieved back from the server
 * 
 * Input: client object, server address, and server message
 *
 * Output: boolean true if gets a quit message from the server
 * and false if not
*/
bool handleMessage(void* arg, const addr_t addr, const char* message) {
	// cast arg to client
	client_t* client = arg;

	// if an OK message is recieved
	if (strncmp(message, "OK ", strlen("OK ")) == 0) {
		sscanf(message, "OK %c", &(client->playerId)); 
	// if a QUIT message is recieved call handleQuit and return true
	} else if (strncmp(message, "QUIT ", strlen("QUIT ")) == 0) {
		handleQuit(message);
		return true;
	// if a GOLD message is recieved call handleGold
	} else if (strncmp(message, "GOLD ", strlen("GOLD ")) == 0) {
		handleGold(message, client);
	// if a GRID message is recieved call handleGrid
	} else if (strncmp(message, "GRID ", strlen("GRID ")) == 0) { 
		handleGrid(message);
	// if DISPLAY message is recieved call handleDisplay
	} else if (strncmp(message, "DISPLAY", strlen("DISPLAY")) == 0) {
		handleDisplay(message);
	// if ERROR message is recieved call handleError
	} else if (strncmp(message, "ERROR ", strlen("ERROR ")) == 0) {
		handleError(message);
	// if an invalid message recieved call handleInvalidMessage
	} else {
		handleInvalidMessage();
	}

	return false;
}

/**************** handleQuit() ****************/
/*
 * The handleQuit function quits the game and
 * is called by handleMessage
 * 
 * Input: message from the server
 *
 * Output: none
*/
void handleQuit(const char* message) {
	// end ncurses

	nocbreak();
	endwin();
	// build status message
	char* copy = mem_assert(mem_malloc(strlen(message)+1), "allocating for handle quit message copy");
	strcpy(copy, message);
	char* status = copy + strlen("QUIT ");
	// log status message to stderr
	log_s("%s\n", status);
	// free the copy
	mem_free(copy);
}

/**************** handleGold() ****************/
/*
 * The handleGold function prints a status message
 * about the gold in the game and is called by handleMessage
 * 
 * Input: message from the server
 *
 * Output: none
*/
void handleGold(const char* message, client_t* client) {
	// initialize n (n gold nuggets), p (purse has p nuggets), and r (r nuggest remaining)
	int n, p, r;
	// scan the message for n, p, and r
	sscanf(message, "GOLD %d %d %d", &n, &p, &r);

	// if the client is a spectator
	if (client->isSpectator) {
		// print the number of nuggests left as status
		mvprintw(0, 0, "Spectator: %d nuggets unclaimed", r);
	} else {
		// print the player's name number of nuggets collected and the number left.
		mvprintw(0, 0, "Player %c has %d nuggets (%d nuggets unclaimed).", client->playerId, p, r);
		if (n > 0) {
			mvprintw(0, maxStatusMessageLength, "GOLD received: %d", n);
		}
	}
	// clear and re-draw
	clrtoeol();
	refresh();
}

/**************** handleGrid() ****************/
/*
 * handleGrid Validates and updates the size of the grid 
 * bases on message from the server
 *
 * Input: message from server
 *
 * Output: none
*/
void handleGrid(const char* message) {
	// initialize screen dimensions and grid dimensions
	int screenWidth, screenHeight;
	int gridWidth, gridHeight;
	// obtain grid height and width from the server message
	sscanf(message, "GRID %d %d", &gridHeight, &gridWidth);

	// get the screen height and width
	getmaxyx(stdscr, screenHeight, screenWidth);

	// prompt the user to resize the screen and continue once they do
	while(screenWidth < (gridWidth + 1) || screenHeight < (gridHeight + 1)) {
		// print resize message as status
		mvprintw(0, 0, "Your display window is too small. It must be allow for least %d characters wide and %d characters high. Resize and press the enter key to continue playing", gridHeight + 1, gridWidth + 1); 
		
		// if enter key is pressed get the new screen dimensions
		char keyPressed = getch();
		if (keyPressed == '\n') {
			getmaxyx(stdscr, screenHeight, screenWidth);
		}
	}
	
	// move cursor to 0,0 clear the status message and re-draw
	move(0, 0);
	clrtoeol();
	refresh();
}

/**************** handleDisplay() ****************/
/*
 * The handleDislpay function is used by handleMessage to
 * handle the display
 * 
 * 
 * Input: message from the server
 *
 * Output: none
*/
void handleDisplay(const char* message) {
	// extract the map message
    const char* map = message + strlen("DISPLAY\n");

    // get the current cursor position
    int begY, begX, currY, currX;
    getyx(stdscr, currY, currX);
    getbegyx(stdscr, begY, begX);
    begY = currY + 1;

    // print the map message
    move(begY, 0);
    for (int i = 0; i < strlen(map); i++) {
        char c = map[i];
        if (c == '\n') {
            begY++;
            currX = begX;
            move(begY, currX);
        } else {
            addch(c);
            currX++;
        }
    }
	// re-draw
    refresh();

    // move to top left
    move(0, 0);
}


/**************** handleError() ****************/
/*
 * The handleError function is used by handleMessage to
 * print error messages
 * 
 * Input: message from the server
 *
 * Output: None
*/
void handleError(const char* message) {
	// allocate memory for and create a copy of the message
	char* copy = mem_assert(mem_malloc(strlen(message) + 1), "allocating error message");
	strcpy(copy, message);
	// build a status message with the message copy
	char* status = copy + strlen("ERROR ");
	// print the status message
	mvprintw(0, maxStatusMessageLength, "%s", status);
	// log the error status message
	log_s("%s", status);
	// redraw the display
	refresh();
	// free the message copy memory
	mem_free(copy);
}

/**************** handleInvalidMessage() ****************/
/*
 * The handleInvalid function is used by handleMessage to
 * handle and invalid message recieved from the server. It prints
 * a status and logs the status to stderr.
 * 
 * Input: None
 *
 * Output: None
*/
void handleInvalidMessage() {
	// print status
	mvprintw(0, 0, "ERROR: received an invalid message");
	// log status
	log_v("ERROR: received an invalid message");

	// clear status and re-draw
	clrtoeol();
	refresh();
}