#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <string.h>
#include "message.h"
#include "log.h"
#include <ctype.h>
#include <unistd.h>
#include "memory.h"
#include "maps.h"
#include "player.h"

#define MaxNameLength 50   // max number of chars in playerName
#define MaxPlayers 26      // maximum number of players
#define GoldTotal 250      // amount of gold in the game
#define GoldMinNumPiles 10 // minimum number of gold piles
#define GoldMaxNumPiles 30 // maximum number of gold piles

/* A data structure which holds the player, their address, and their letter */
typedef struct playerNode
{
        addr_t address;
        char *playerLetter;
        player_t *player;
} playerNode_t;

/* A data structure which holds the player, their address, and their letter */
playerNode_t *playerNode_new()
{
        playerNode_t *playerNode = malloc(sizeof(playerNode_t));
        return playerNode;
}

/* A data structure which holds all of the players */
typedef struct playerTable
{
        playerNode_t *arr[MaxPlayers];
} playerTable_t;

typedef struct spectator
{
        addr_t address;
} spec_t;

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
/* A global variable game which holds the most important states of the game */
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

/****************new_gold_pile****************/
gold_pile_t *new_gold_pile(int size)
{
  gold_pile_t *pile = malloc(sizeof(gold_pile_t)); //allocate space for pile
  if (pile == NULL)
  {
    return NULL;
  }
  else
  {
    pile->amount = size;
  }
  return pile;
}
/****************delete_gold_pile****************/
void delete_gold_pile(gold_pile_t *pile)
{
  free(pile);
}

/* Create a new gold */
gold_t *gold_new(int numberOfPiles)
{
        gold_t* goldList = mem_malloc_assert(sizeof(gold_t), "gold_new: unable to allocate memory for gold");
        (*goldList).num_piles = numberOfPiles;
        (*goldList).piles = new_gold_pile((*goldList).num_piles);
        return goldList;
}

/* Create a new spectator */
spec_t *spectator_new(addr_t address)
{
        spec_t *spectator = malloc(sizeof(spec_t));
        spectator->address = address;
        return(spectator);
}

/* Create a new player table */
playerTable_t *playerTable_new()
{
        playerTable_t *playerTable = malloc(sizeof(playerTable_t));
        for (int i = 0; i < MaxPlayers; i++)
        {
                playerTable->arr[i] = NULL;
        }
        return playerTable;
}

/* Deletes everything in the player table */
void playerTable_delete(playerTable_t *playerTable, int yRange)
{
        for (int i = 0; i < MaxPlayers; i++)
        {
                // loop through everything in the array
                if (playerTable->arr[i] != NULL)
                {
                        player_delete(playerTable->arr[i]->player, yRange);
                        free(playerTable->arr[i]);
                }
        }
        free(playerTable);
}


/* Constructor to create a new counter array node */
game_t *gamenode_new(int num_nuggets, map_t *curr_map)
{
        game_t *game = calloc(1, sizeof(game_t));

        // Error check
        if (game == NULL)
        {
                fprintf(stderr, "Failed to create the new game.\n");
                return NULL;
        }
        else
        {
                // fill in parameter initializations
                game->playersChar = NULL;
                game->numplayers = 0;
                game->num_nuggets = num_nuggets;
                game->spectator = NULL;
                game->map = curr_map;
                game->players = playerTable_new();
                return game;
        }
}

game_t *game;
/**************** main ****************/
/*
Main performs a series of checks and initializes the necessary components before starting the game. 
It validates the number of arguments passed to the program, 
initializes the logging file, and sets up the game environment based on the arguments provided. 
Finally, it starts the server and waits for clients to connect. Once the game is complete,
 it cleans up any resources used by the program and exits.
*/
int main(int argc, char *argv[])
{
        // Validate the number arguments
        if ((argc < 2) || (argc > 3))
        {
                fprintf(stderr, "Please input one or two parameters. The map txt, the optional seed\n");
                fprintf(stderr, "The number of inputted arguments: %d\n", argc);
                return 1;
        }

        // Can assume a map file is valid, but need to infer, but must infer *NR* and *NC* by reading the file.
        // A *map file* is a text file with exactly *NC* lines and in which every line has exactly *NR* characters.
        char *map_pathname = (char *)calloc(100, sizeof(char));
        strcpy(map_pathname, ((char *)argv[1]));

        // Initialize the logging file
        log_init(stderr);

        //If provided, check that the seed is a positive integer
        int num_nuggets;
        if ((argc == 3) || (argc == 4))
        {
                if (argv[2] != NULL)
                {
                        // get the seed
                        int input;
                        input = atoi(argv[2]);

                        // when number passed => 1, otherwise, 0
                        if (isNumber(argv[2]) == 0)
                        {
                                fprintf(stderr, "Please enter an integer seed value.\n");
                                return 3;
                        }
                        // if the seed is not positive
                        if (input < 1)
                        {
                                fprintf(stderr, "Please enter a positive seed value.\n");
                                return 2;
                        }

                        // Pass the seed to `srand(seed)`
                        // seed the random-number generator with the time of day
                        srand(input);
                        num_nuggets = (rand() % GoldMaxNumPiles);

                        // make sure the number of nuggets is within bounds
                        while ((num_nuggets < GoldMinNumPiles) || (num_nuggets > GoldMaxNumPiles))
                        {
                                num_nuggets = (rand() % GoldMaxNumPiles);
                        }
                }
        }
        else
        {
                // Pass the seed to `srand(seed)`
                // seed the random-number generator with the time of day
                srand(getpid());
                num_nuggets = (rand() % GoldMaxNumPiles);

                // make sure the number of nuggets is within bounds
                while ((num_nuggets < GoldMinNumPiles) || (num_nuggets > GoldMaxNumPiles))
                {
                        num_nuggets = (rand() % GoldMaxNumPiles);
                }
        }


        // // Initialize and load the map
        map_t *curr_map = maps_new(map_pathname);
        if (curr_map == NULL) {
          log_v("Got new map to be NULL - stopping game");
          return 2;
        }
        free(map_pathname);
        server_dropGold(curr_map, num_nuggets, GoldTotal);

        // Initialize the game struct
        game = gamenode_new(0, curr_map);
        // Initialize the network -- did I initiaize the network correctly?
        int port_num = message_init(stderr);

        if (port_num == 0)
        {
                fprintf(stderr, "Failed to create a port number.\n");
                // Log error and return zero if any error.
                log_e("Failed to generate a port number\n");
                return 0;
        }
        else
        {
                // Announce the port number
                log_d("The port number is %d\n", port_num);
        }

        // Determine portstring
        char portStr[100];
        sprintf(portStr, "%d", port_num);

        // Announce the port
        log_d("Annoucing the port! Ready at port '%d'", port_num);

        //Create an internet address; initialize in in a call to message_setAddr
        addr_t addr = message_noAddr(); //calloc(1, sizeof(addr_t));

        message_loop(&addr, 0, NULL, NULL, handleMessage);
        log_v("thanks for playing!");

        // Close the server
        message_done();

        // Call log_done() before the program exits
        log_done();
        return 0;
}

/**************** server_dropGold ****************/
/* A function called to drop random piles of gold in the map */
void server_dropGold(map_t *map, int num_piles, int gold_amount)
{

  maps_setTotalGoldLeft(map,gold_amount);
  int xRange = maps_getXrange(map);
  log_d("server_dropGold: got xRange in map to be %d", xRange);
  int yRange = maps_getYrange(map);
  log_d("server_dropGold: got yRange in map to be %d", yRange);
  
  //Counting the amount of open spaces where gold can go
  int open_spaces = 0;
  for (int i = 0; i < xRange; i++)
  {
    for (int j = 0; j < yRange; j++)
    {
      if (maps_getMapNode(map, i, j) != NULL)
      {
        if ( maps_getMapNodeItem(maps_getMapNode(map, i, j)) == '.')
        {
          open_spaces++;
        }
      }
    }
  }

  int max_num_piles;
  if (num_piles > open_spaces - MaxPlayers)
  {
    max_num_piles = open_spaces - MaxPlayers; //To allow for MaxPlayers players
  }
  else
  {
    max_num_piles = num_piles;
  }

  int gold_left = gold_amount;
  int piles_left = max_num_piles;

  //while there are still piles to add
  for (int m = 0; m < max_num_piles; m++)
  {
    int pile_gold;
    if (piles_left == 1)
    { //put 1 gold in the pile if there is only one pile left
      pile_gold = gold_left;
    }
    else
    { //put random amount of gold of at least 1 in pile, but not so much that there isn't enough gold for the rest of the piles
     // srand(getppid());
      pile_gold = 1 + (rand() % ((gold_left - piles_left + 1) / 2)); //Divide max by 2 so as to not get disproportionately large piles
    }

    gold_left = gold_left - pile_gold;

    //genreate random gold location
    int gold_location = (rand() % open_spaces);

    //Loop through, and add gold to this location
    int space = 0;
    for (int i = 0; i < xRange; i++)
    {
      for (int j = 0; j < yRange; j++)
      {
        if (maps_getMapNode(map, i, j) != NULL)
        {
          if ( maps_getMapNodeItem(maps_getMapNode(map, i, j)) == '.')
          {
            if (space == gold_location)
            {
               maps_setMapNodeItem(maps_getMapNode(map, i, j), '*');
              gold_pile_t *pile = new_gold_pile(pile_gold);
               maps_setMapNodeType(maps_getMapNode(map, i, j), pile);
            }
            space++;
          }
        }
      }
    }

    //Decrement and change values
    piles_left--;
    open_spaces--;
  }
}

/**************** handleMessage ****************/
/*
This function  is a utility function that allows a server to handle 
incoming messages from clients and take appropriate actions based on the 
contents of the message.
*/
bool handleMessage(void *arg, const addr_t from, const char *message)
{


        // Check if the port is null
        addr_t *otherp = (addr_t *)arg;

        if (otherp == NULL)
        {
                log_v("handleMessage called with arg=NULL");
                return true;
        }

        // this sender becomes our correspondent, henceforth
        *otherp = from;
        
        if (strncmp(message, "PLAY ", strlen("PLAY ")) == 0)
        {
                log_d("the num players %d", game->numplayers);
                // check and make sure there are not more than MaxPlayers users
                if (game->numplayers >= MaxPlayers)
                {
                        fprintf(stderr, "Sorry, no more than MaxPlayers=%d players can join! Please try again later.\n", MaxPlayers);
                        char *message_to_send = "QUIT Sorry, we've reached the max number of players in the server! Please try joining a different server.\n";
                        message_send(from, message_to_send);
                        return false;
                }

                char *real_name;
                // Normalize the player's name
                const char separator = ' ';
                char *const sep_at = strchr(message, separator);

                if (sep_at != NULL)
                {
                        // Create
                        *sep_at = '\0'; /* overwrite first separator, creating two strings. */
                        real_name = sep_at + 1;
                }

                // check if the real name is null
                if (real_name == NULL)
                {
                        fprintf(stderr, "Sorry, please enter a valid real name.\n");
                        return 7;
                }

                /*
                // make sure the name is truncated to 20 char
                if (strlen(real_name) > 20)
                {
                        int diff = strlen(real_name) - 20;
                        real_name[strlen(real_name) - diff] = '\0';
                }
                */

                // Create new player
                player_t *newPlayer = player_new(real_name, from, MaxNameLength, maps_getXrange(game->map), maps_getYrange(game->map), '@');
                newPlayer = player_set(game->map, newPlayer);

                // Add the player to the game struct array
                server_dropPlayer(game->map, newPlayer);
                playerNode_t *node = playerNode_new();
                node->player = newPlayer;

                // add the player to the array by determing the index to add to
                int i = 0;
                while (((game->players->arr[i]) != NULL) && (i < MaxPlayers))
                {
                        i++;
                }

                // Assign the player to arr[i] and add the player letter 'A'
                (game->players->arr[i]) = node;
                char playerLetter;

                log_d("The index: %d\n", i);

                // Add their corresponding char for the spectator and end of game summary
                if (game->players->arr[i] != NULL)
                {
                        log_v("Successfully added a player\n");

                        // Determine their characters
                        playerLetter = getCharacterBasedOnIndex(i);
                        log_c("The character letter %c\n", playerLetter);
                        //server_dropPlayer_char(newPlayer, *playerLetter);
                        player_setLetterAssigned(game->players->arr[i]->player, playerLetter);
                        //game->players->arr[i]->playerLetter = playerLetter;
                }

                //Increment the number of players
                game->numplayers++;

                // Respond with OK 'char letter'
                char letter[100] = "OK ";
                letter[3] = playerLetter;

                // Send the message
                message_send(from, letter);

                //Update DISPLAY for all players
                int nrows = maps_getRows(game->map);
                int ncols = maps_getCols(game->map);
                int size2 = strlen("GRID") + 16;

                // Sending map message
                char *map_message = calloc(size2, sizeof(char));
                if (ncols < 30)
                {
                        sprintf(map_message, "GRID %d %d", nrows, 30);
                }
                else
                        sprintf(map_message, "GRID %d %d", nrows, ncols);

                message_send(from, map_message);
                free(map_message);

                // send the player display
               

                send_player_gold(game, newPlayer, from);
                send_player_display(game, newPlayer, from);
                 for (int j = 0; j < game->numplayers; j++)
                {
                        // send all of the players a tabular summary
                        player_t *player = game->players->arr[j]->player;
                        send_player_gold(game, player, player_getIP(game->players->arr[j]->player));
                        send_player_display(game, player, player_getIP(game->players->arr[j]->player));
                }

                return false;
        }

        /* Handle spectator!! */
        if (strncmp(message, "SPECTATE", strlen("SPECTATE")) == 0)
        {
                log_v("A spectator has been added!");

                // if there is already a spectator
                if (game->spectator != NULL)
                {
                        // Send a `QUIT` message to the prior spectator, then forgets the prior spectator
                        char *message_to_send = game_over_summary();
                        message_send(game->spectator->address, message_to_send);
                        free(game->spectator);
                        free(message_to_send);
                }

                // add the new spectator
                // send a `GRID`, `GOLD`, and `DISPLAY` message
                game->spectator = spectator_new(from);

                int nrows = maps_getRows(game->map);
                int ncols = maps_getCols(game->map);
                int size2 = strlen("GRID") + 15 + 1;

                // Sending map message
                char *map_message = calloc(size2, sizeof(char));
                sprintf(map_message, "GRID %d %d", nrows, ncols);
                message_send(from, map_message);
                free(map_message);

                // send the gold and display messages
                send_spectator_gold(game, from);
                send_spectator_display(game, from);

                return false;
        }

        // handle the quit message!
        else if (strncmp(message, "KEY Q", strlen("KEY Q")) == 0)
        {
                log_v("A player is trying to quit.\n");
                char *message_to_send = game_over_summary();
                message_send(from, message_to_send);
                if(!message_eqAddr(from,game->spectator->address))
                {
                maps_setMapNodeItem(maps_getMapNode(game->map, player_getXPosition(searchByAddress(from)),player_getYPosition(searchByAddress(from))),'.');
                maps_setMapNodeType(maps_getMapNode(game->map, player_getXPosition(searchByAddress(from)),player_getYPosition(searchByAddress(from))),NULL);
                for (int j = 0; j < game->numplayers; j++)
                  {
                          // send all of the players a tabular summary
                          player_t *player = game->players->arr[j]->player;
                          send_player_gold(game, player, player_getIP(game->players->arr[j]->player));
                          send_player_display(game, player, player_getIP(game->players->arr[j]->player));
                  }
                }
                free(message_to_send);

                return false;
        }

        /* CASES FOR A PLAYER */
        // Check all of the above cases and handle accordingly
        else if ((strcmp(message, "KEY H") == 0))
        {
                move_left_MAX(game->map, searchByAddress(from));
        }
        else if (strcmp(message, "KEY h") == 0)
        {
                move_left(game->map, searchByAddress(from));
        }
        else if ((strcmp(message, "KEY L") == 0))
        {
                move_right_MAX(game->map, searchByAddress(from));
        }
        else if ((strcmp(message, "KEY l") == 0))
        {
                move_right(game->map, searchByAddress(from));
        }
        else if ((strcmp(message, "KEY J") == 0))
        {
                move_down_MAX(game->map, searchByAddress(from));
        }
        else if (strcmp(message, "KEY j") == 0)
        {
                move_down(game->map, searchByAddress(from));
        }
        else if ((strcmp(message, "KEY K") == 0))
        {
                move_up_MAX(game->map, searchByAddress(from));
        }
        else if (strcmp(message, "KEY k") == 0)
        {
                move_up(game->map, searchByAddress(from));
        }
        else if ((strcmp(message, "KEY Y") == 0))
        {
                move_diag_up_left_MAX(game->map, searchByAddress(from));
        }
        else if (strcmp(message, "KEY y") == 0)
        {
                move_diag_up_left(game->map, searchByAddress(from));
        }
        else if ((strcmp(message, "KEY U") == 0))
        {
                move_diag_up_right_MAX(game->map, searchByAddress(from));
        }
        else if (strcmp(message, "KEY u") == 0)
        {
                move_diag_up_right(game->map, searchByAddress(from));
        }
        else if ((strcmp(message, "KEY B") == 0))
        {
                move_diag_down_left_MAX(game->map, searchByAddress(from));
        }
        else if (strcmp(message, "KEY b") == 0)
        {
                move_diag_down_left(game->map, searchByAddress(from));
        }
        else if ((strcmp(message, "KEY N") == 0))
        {
                move_diag_down_right_MAX(game->map, searchByAddress(from));
        }
        else if (strcmp(message, "KEY n") == 0)
        {
                move_diag_down_right(game->map, searchByAddress(from));
        }
        else
        {
                fprintf(stderr, "Please enter a valid letter command.\n");
                log_v("Please enter a valid letter command.\n");
                return false;
        }

        // Check if there is no gold left
        int gold_left = maps_getTotalGoldLeft(game->map);
        if (gold_left == 0)
        {
                // if so, print the tabular summary to all of the players
                for (int j = 0; j < game->numplayers; j++)
                {
                        // send all of the players a tabular summary
                        addr_t new_address = player_getIP(game->players->arr[j]->player);
                        char *message_to_send = game_over_summary();
                        message_send(new_address, message_to_send);
                        free(message_to_send);
                }
                // as well as the spectator if applicable
                if (game->spectator != NULL)
                {
                        // send all of the players a tabular summary
                        addr_t new_address = game->spectator->address;
                        char *message_to_send = game_over_summary();
                        message_send(new_address, message_to_send);
                        free(message_to_send);
                }
                free_everything(game);
                return true;
        }
        else
        {
                for (int j = 0; j < game->numplayers; j++)
                {
                        // send all of the players a tabular summary
                        player_t *player = game->players->arr[j]->player;
                        send_player_gold(game, player, player_getIP(game->players->arr[j]->player));
                        send_player_display(game, player, player_getIP(game->players->arr[j]->player));
                }

                if (game->spectator != NULL)
                {
                        // send the spectator a tabular summary
                        send_spectator_gold(game, game->spectator->address);
                        send_spectator_display(game, game->spectator->address);
                }

                return false;
        }
}


/**************** send_player_display ****************/
/*
The function is to send a message containing information 
about the display left in the game to a player.
*/
void send_player_display(game_t *game, player_t *player, addr_t from)
{
        // create the grid string and determine the size
        char *mapstring = maps_playergrid(game->map, player);
        int size = strlen(mapstring) + strlen("DISPLAY\n") + 1;
        char *DISPLAYMESSAGE = calloc(size, sizeof(char));

        // begin sending the message
        strcpy(DISPLAYMESSAGE, "DISPLAY\n");
        strcat(DISPLAYMESSAGE, mapstring);
        message_send(from, DISPLAYMESSAGE);

        // free
        free(mapstring);
        free(DISPLAYMESSAGE);
}

/**************** send_player_gold ****************/
/*
The function is to send a message containing information 
about the amount of gold left in the game to a player.
*/
void send_player_gold(game_t *game, player_t *player, addr_t from)
{
        char *goldmessage = createGoldMessage(game->map, player);
        message_send(from, goldmessage);
        free(goldmessage);
}

/**************** send_spectator_display ****************/
/* The function is to send a message containing information 
about the display left in the game to a spectator. */
void send_spectator_display(game_t *game, addr_t from)
{
        // create the grid string and determine the size
        char *mapstring = maps_spectatorgrid(game->map);
        int size = strlen(mapstring) + strlen("DISPLAY\n") + 1;
        char *DISPLAYMESSAGE = calloc(size, sizeof(char));

        // begin sending the message
        strcpy(DISPLAYMESSAGE, "DISPLAY\n");
        strcat(DISPLAYMESSAGE, mapstring);
        message_send(from, DISPLAYMESSAGE);

        // free
        free(mapstring);
        free(DISPLAYMESSAGE);
}

/**************** send_spectator_gold ****************/
/* The function is to send a message containing information 
about the gold amount left in the game to a spectator. */
void send_spectator_gold(game_t *game, addr_t from)
{
        // create the grid string and determine the size
        int len = strlen("GOLD ") + 15 + 1;
        char *goldmessage_spectator = calloc(len, sizeof(char));

        // determine gold comp
        int n = 0;
        int p = 0;
        int r = maps_getTotalGoldLeft(game->map);

        // send and free
        sprintf(goldmessage_spectator, "GOLD %d %d %d", n, p, r);
        message_send(from, goldmessage_spectator);
        free(goldmessage_spectator);
}
/**************** createGoldMessage ****************/
char *createGoldMessage(map_t *map, player_t *player)
{
  char *message = malloc(20 * sizeof(char)); //allocate at least 20 chars of space
  int just_collected = player_getJustCollected(player); //Need to check with Jake on this
  int collected = player_getGold(player);
  int left = maps_getTotalGoldLeft( map);
  sprintf(message, "GOLD %d %d %d\n", just_collected, collected, left); //add info to message
  return message; //return message
}

player_t *player_set(map_t *map, player_t *player)
{
  if (map == NULL) {
    log_v("player_set: got map to be NULL");
    return NULL;
  }
  //Loop through grid to determine amount of open spaces
  int open_spaces = 0;
  for (int i = 0; i < maps_getXrange(map); i++)
  {
    for (int j = 0; j < maps_getYrange(map); j++)
    {
      if (maps_getMapNode(map, i, j) != NULL)
      {
        if (maps_getMapNodeItem(maps_getMapNode(map, i, j)) == '.')
        {
          open_spaces++;
        }
      }
    }
  }

  //generate random player location index
  //srand(getppid());
  int player_location = (rand() % open_spaces);

  //Loop through open spaces and place player there
  int space = 0;
  for (int i = 0; i < maps_getXrange(map); i++)
  {
    for (int j = 0; j < maps_getYrange(map); j++)
    {
      if (maps_getMapNode(map, i, j) != NULL)
      {
        if ( maps_getMapNodeItem(maps_getMapNode(map, i,j)) == '.')
        {
          if (space == player_location)
          {
            player_setYPosition(player, j);
            player_setXPosition(player, i);
          }
          space++;
        }
      }
    }
  }

  //set whole grid to not visible
  for (int i = 0; i < maps_getXrange(map); i++)
  {
    for (int j = 0; j < maps_getYrange(map); j++)
    {
      player_addSeenMap(player, i, j, false);
    }
  }
  return player;
}


/* Returns the alphabetical letter based on index */
char getCharacterBasedOnIndex(int i)
{
    char toReturn;
    if (i < 0 || i > 25)
    {
        fprintf(stderr, "Please enter a valid index number.\n");
        toReturn = '\0';
    }
    else
    {
      toReturn = (i + 65);
    }
    return toReturn;
}

/**************** move_left ****************/
/* Function to check if the location to move is moveable and calls player_move */
void move_left(map_t *map, player_t *player)
{
  if (player_getXPosition(player) >= 1)
  {
    if (moveable(maps_getMapNode(map,player_getXPosition(player)-1,player_getYPosition(player))))
    {
      player_move(map, player, player_getXPosition(player) - 1, player_getYPosition(player));
    }
  }
}

void move_left_MAX(map_t *map, player_t *player)
{
  while ((moveable(maps_getMapNode(map,player_getXPosition(player)-1,player_getYPosition(player)))) && (player_getXPosition(player) >= 1))
  {
    player_move(map, player, player_getXPosition(player) - 1, player_getYPosition(player));
  }
}

/**************** move_right ****************/
/* Function to check if the location to move is moveable and calls player_move */
void move_right(map_t *map, player_t *player)
{
  if (player_getXPosition(player) < maps_getCols(map))
  {
    if (moveable(maps_getMapNode(map,player_getXPosition(player)+1,player_getYPosition(player))))
    {
      player_move(map, player, player_getXPosition(player) + 1, player_getYPosition(player));
    }
  }
}

void move_right_MAX(map_t *map, player_t *player)
{
  while ((moveable(maps_getMapNode(map,player_getXPosition(player)+1,player_getYPosition(player)))) && (player_getXPosition(player) < maps_getCols(map)))
  {
    player_move(map, player, player_getXPosition(player) + 1, player_getYPosition(player));
  }
}

/**************** move_up ****************/
/* Function to check if the location to move is moveable and calls player_move */
void move_up(map_t *map, player_t *player)
{
  if (moveable(maps_getMapNode(map,player_getXPosition(player),player_getYPosition(player)-1)))
  {
    player_move(map, player, player_getXPosition(player), player_getYPosition(player) - 1);
  }
}

/**************** move_up_MAX ****************/
/* Function to check if the location to move is moveable and calls player_move iteratively until it can */
void move_up_MAX(map_t *map, player_t *player)
{
  while (moveable(maps_getMapNode(map,player_getXPosition(player),player_getYPosition(player)-1)))
  {
    player_move(map, player, player_getXPosition(player), player_getYPosition(player) - 1);
  }
}

/**************** move_down ****************/
/* Function to check if the location to move is moveable and calls player_move */
void move_down(map_t *map, player_t *player)
{
  if (moveable(maps_getMapNode(map,player_getXPosition(player),player_getYPosition(player)+1)))
  {
    player_move(map, player, player_getXPosition(player), player_getYPosition(player) + 1);
  }
}

/**************** move_down_MAX ****************/
/* Function to check if the location to move is moveable and calls player_move iteratively until it can */

void move_down_MAX(map_t *map, player_t *player)
{
  while (moveable(maps_getMapNode(map,player_getXPosition(player),player_getYPosition(player)+1)))
  {
    player_move(map, player, player_getXPosition(player), player_getYPosition(player) + 1);
  }
}

/**************** move_diag_up_left ****************/
/* Function to check if the location to move is moveable and calls player_move */
void move_diag_up_left(map_t *map, player_t *player)
{
  if (moveable(maps_getMapNode(map,player_getXPosition(player) - 1,player_getYPosition(player) - 1)))
  {
    player_move(map, player, player_getXPosition(player) - 1, player_getYPosition(player) - 1);
  }
}

/**************** move_diag_up_left_MAX ****************/
/* Function to check if the location to move is moveable and calls player_move iteratively until it can */
void move_diag_up_left_MAX(map_t *map, player_t *player)
{
  while (moveable(maps_getMapNode(map,player_getXPosition(player) - 1,player_getYPosition(player) - 1)))
  {
    player_move(map, player, player_getXPosition(player) - 1, player_getYPosition(player) - 1);
  }
}

/**************** move_left ****************/
/* Function to check if the location to move is moveable and calls player_move */
void move_diag_up_right(map_t *map, player_t *player)
{
  if (moveable(maps_getMapNode(map,player_getXPosition(player) + 1,player_getYPosition(player) - 1)))
  {
    player_move(map, player, player_getXPosition(player) + 1, player_getYPosition(player) - 1);
  }
}

/**************** move_diag_up_right_MAX ****************/
/* Function to check if the location to move is moveable and calls player_move iteratively until it can */
void move_diag_up_right_MAX(map_t *map, player_t *player)
{
  while (moveable(maps_getMapNode(map,player_getXPosition(player) + 1,player_getYPosition(player) - 1)))
  {
    player_move(map, player, player_getXPosition(player) + 1, player_getYPosition(player) - 1);
  }
}

/**************** move_left ****************/
/* Function to check if the location to move is moveable and calls player_move */
void move_diag_down_left(map_t *map, player_t *player)
{
  if (moveable(maps_getMapNode(map,player_getXPosition(player) - 1,player_getYPosition(player) + 1)))
  {
    player_move(map, player, player_getXPosition(player) - 1, player_getYPosition(player) + 1);
  }
}

/**************** move_diag_down_left_MAX ****************/
/* Function to check if the location to move is moveable and calls player_move iteratively until it can */
void move_diag_down_left_MAX(map_t *map, player_t *player)
{
  while (moveable(maps_getMapNode(map,player_getXPosition(player) - 1,player_getYPosition(player) + 1)))
  {
    player_move(map, player, player_getXPosition(player) - 1, player_getYPosition(player) + 1);
  }
}

/**************** move_left ****************/
/* Function to check if the location to move is moveable and calls player_move */
void move_diag_down_right(map_t *map, player_t *player)
{
  if (moveable(maps_getMapNode(map,player_getXPosition(player) + 1,player_getYPosition(player) + 1)))
  {
    player_move(map, player, player_getXPosition(player) + 1, player_getYPosition(player) + 1);
  }
}

/**************** move_diag_down_right_MAX ****************/
/* Function to check if the location to move is moveable and calls player_move iteratively until it can */
void move_diag_down_right_MAX(map_t *map, player_t *player)
{
  while (moveable(maps_getMapNode(map,player_getXPosition(player) + 1,player_getYPosition(player) + 1)))
  {
    player_move(map, player, player_getXPosition(player) + 1, player_getYPosition(player) +1);
  }
}

/**************** moveable ****************/
/*
his function determines whether a given node on a map is moveable or not.
It takes a pointer to a map node as input and returns a boolean value indicating
whether it is safe to move onto the node or not. 
The function checks if the node is not null and if it contains a valid character ('.', '*', '@', or '#') 
which denotes empty space, a box, the player, or a target respectively.
*/
bool moveable(mapNode_t *node)
{
  mapNode_t *this_node = node;
  char c;
  if (this_node != NULL)
  {
    c = maps_getMapNodeItem(this_node);
  }
  else
  {
    return false;
  }
  if (c == '.' || c == '*' || c == '@' || c == '#')
  {
    return true;
  }
  return false;
}

/********************* server_dropPlayer ********************/
void server_dropPlayer(map_t *map, player_t *player)
{
  maps_setMapNodeItem(maps_getMapNode(map, player_getXPosition(player),player_getYPosition(player)), '@');
  maps_setMapNodeType(maps_getMapNode(map, player_getXPosition(player),player_getYPosition(player)), player);
  make_visible(player, map);
}


/********************* player_move ********************/
/*
This function  is responsible for moving the player within the game map. 
It takes the current map, player object, and the new x and y coordinates to move the player to.
The function checks for the type of the item present in the new position and performs different actions based on it.
If it's a gold pile, it subtracts the gold from the map and adds it to the player's inventory.
If it's a hallway, it simply switches the character with an empty space. 
If it's another player, it swaps the positions of the two players.
Finally, the function updates the game map with the new positions of the players and the items
*/
void player_move(map_t *map, player_t *player, int new_x, int new_y)
{
  int x = player_getXPosition(player);
  int y = player_getYPosition(player);

  char char_to_switch;  //Character that the player is switching with
  void *item_to_switch; //Type that the character is switching with

  if (maps_getMapNodeItem(maps_getMapNode(map, new_x,new_y)) == '*')
  { //If a pile of gold
    gold_pile_t *pile = maps_getMapNodeType(maps_getMapNode(map, new_x,new_y));
    
    maps_setTotalGoldLeft(map,maps_getTotalGoldLeft(map)-pile->amount); //subtract amoun of gold from goldleft
    player_setJustCollected(player, pile->amount);
    player_addGold(player,pile->amount);
    char_to_switch = '.';  //Player is switching with empty space
    item_to_switch = NULL; //Player is switching with null type
    delete_gold_pile(pile);
  }
  else if (maps_getMapNodeItem(maps_getMapNode(map, new_x,new_y))  == '#')
  { //if going down a hallway
    //same (hallway boolean in player takes care of hallway print)
    char_to_switch = '.';
    item_to_switch = NULL;
  }
  else if (maps_getMapNodeItem(maps_getMapNode(map, new_x,new_y)) == '@')
  { //swap player
    player_t *swapped_player = (player_t *)(maps_getMapNodeType(maps_getMapNode(map, new_x,new_y)));
    player_setXPosition(swapped_player, x);
    player_setYPosition(swapped_player, y);
    char_to_switch = maps_getMapNodeItem(maps_getMapNode(map, new_x,new_y)); //character at new place
    item_to_switch = maps_getMapNodeType(maps_getMapNode(map, new_x,new_y)); //type at new place
    player_addGold(player,player_getGold(swapped_player));
    player_setJustCollected(player, player_getGold(swapped_player));
    player_addGold((player_t *)(maps_getMapNodeType(maps_getMapNode(map, new_x,new_y))),player_getGold(swapped_player)*-1);
  }
  else
  {
    char_to_switch = maps_getMapNodeItem(maps_getMapNode(map, new_x,new_y)); //character at new place
    item_to_switch = maps_getMapNodeType(maps_getMapNode(map, new_x,new_y)); //type at new place
  }

  //set player coords to new spot
  player_setXPosition(player, new_x);
  player_setYPosition(player, new_y);

  //set things at new spot to the '@' character and player
  maps_setMapNodeItem(maps_getMapNode(map, new_x,new_y),'@');
  maps_setMapNodeType(maps_getMapNode(map, new_x,new_y),player);

  if (maps_isHallwayNode(maps_getMapNode(map,x,y)))
  { //If a hallway is under player
    //Keep it that way
    maps_setMapNodeItem(maps_getMapNode(map, x,y),'#');
    maps_setMapNodeType(maps_getMapNode(map, x,y),NULL);
  }
  else
  { //If not
    //Put the new charcter and item where player is
    maps_setMapNodeItem(maps_getMapNode(map, x,y),char_to_switch);
    maps_setMapNodeType(maps_getMapNode(map, x,y),item_to_switch);
  }

  make_visible(player, map);
}

/* Creates an end game summary for all players */
char *game_over_summary()
{
        // create the string
        char *message_to_send = (char *)mem_calloc_assert(1000, sizeof(char), "Unable to allocate game over summary string memory");
        strcpy(message_to_send, "QUIT ");

        strcat(message_to_send, "Thanks for playing!\n");

        // print the heading
        strcat(message_to_send, "Player Letter\tPlayer Purse\tPlayer Name\n");

        strcat(message_to_send, "-------------\t------------\t-----------\n");

        int temp_num_players = game->numplayers;

        // iterate through the players and add their information
        while (temp_num_players > 0)
        {
                // get the information needed to print
                char player_letter = player_getLetterAssigned(game->players->arr[temp_num_players - 1]->player);
                log_c("Got player letter to be: %c",player_letter);

                int player_purse = (player_getGold(game->players->arr[temp_num_players - 1]->player));
                char *player_purse_char = (char *)calloc(11, sizeof(char));
                sprintf(player_purse_char, "%d", player_purse);

                char *real_name = (char *)calloc(MaxNameLength+1, sizeof(char));
                strcpy(real_name, player_getRealName(game->players->arr[temp_num_players - 1]->player));

                // Add the portions of the table
                //strcat(message_to_send, "      %-5c%-5d%-20s", player_letter)

                char player_letter_string[2] = {player_letter,'\0'};
                strcat(message_to_send, "      ");
                strcat(message_to_send, player_letter_string);

                strcat(message_to_send, "      \t");
                strcat(message_to_send, player_purse_char);

                strcat(message_to_send, "\t\t");
                strcat(message_to_send, real_name);
                strcat(message_to_send, "\n");

                // decrement num players to iterate through and free
                temp_num_players--;
                free(player_purse_char);
                free(real_name);
        }

        log_v(message_to_send);

        return message_to_send;
}

/* Search for a player based on their address  */
player_t *searchByAddress(addr_t from)
{
        int j = 0;

        // iterate through everyone
        while ((message_eqAddr(player_getIP(game->players->arr[j]->player), from) != true) && (j < game->numplayers))
        {
                j++;
        }

        //Once the right address has been found
        player_t *player = game->players->arr[j]->player;
        return (player);
}

/********************* make_visible ********************/
void make_visible(player_t *player, map_t *map)
{
  //loop through all nodes in grid
  for (int x = 0; x < maps_getXrange(map); x++)
  {
    for (int y = 0; y < maps_getYrange(map); y++)
    {
      bool** seen = player_getSeenMap(player);
      if (!(seen[y][x]))
      {
        if (maps_isVisible(map, player_getXPosition(player), player_getYPosition(player), x, y)) //check visiblity of gridnode
        {
          player_addSeenMap(player,x,y,true);
        }
      }
    }
  }
}

void free_everything(game_t *game)
{
        if (game->spectator != NULL)
        {
              free(game->spectator);
        }

        // free

        playerTable_delete(game->players, maps_getYrange(game->map));
}

bool isNumber(char number[])
{
        // iterate through it all
        for (int i = 0; i < strlen(number); i++)
        {
                // use the isDigit method to check input is between 0 and 9
                if (!isdigit(number[i]))
                {

                        fprintf(stderr, "Please enter a number\n");
                        return false;
                }
        }
        return true;
}