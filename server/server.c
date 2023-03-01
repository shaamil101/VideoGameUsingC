#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "map.h"
#include "memory.h"

static const int MaxNameLength = 50;   // max number of chars in playerName
static const int MaxPlayers = 26;      // maximum number of players
static const int GoldTotal = 250;      // amount of gold in the game
static const int GoldMinNumPiles = 10; // minimum number of gold piles
static const int GoldMaxNumPiles = 30; // maximum number of gold piles

/* A data structure which holds the player, their address, and their letter */
typedef struct playerNode
{
        addr_t address;
        char *playerLetter;
        player_t *player;
} playerNode_t;

/* A data structure which holds the player, their address, and their letter */
playerNode _t *playerNode_new()
{
        playerNode_t *playerNode = malloc(sizeof(playerNode_t));
        return playerNode;
}

/* A data structure which holds all of the players */
typedef struct playerTable
{
        playerNode_t *arr[26];
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
        gold_t goldList;
        goldList.num_piles = numberOfPiles;
        gold_t *goldList.piles = malloc(goldList.num_piles * sizeof(gold_pile_t));
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
        for (int i = 0; i < 26; i++)
        {
                playerTable->arr[i] = NULL;
        }
        return playerTable;
}

/* Deletes everything in the player table */
void playerTable_delete(playerTable_t *playerTable)
{
        for (int i = 0; i < 26; i++)
        {
                // loop through everything in the array
                if (playerTable->arr[i] != NULL)
                {
                        player_delete(playerTable->arr[i]->player);
                }
        }
        free(playerTable);
}
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
        map_t *curr_map = maps_new();

        // Load the map and check that the file is readable
        load_map(curr_map, map_pathname);
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
void server_dropGold(map_t *map, int num_piles, int gold_amount)
{

  map->goldleft = gold_amount;

  //Counting the amount of open spaces where gold can go
  int open_spaces = 0;
  for (int i = 0; i < 300; i++)
  {
    for (int j = 0; j < 300; j++)
    {
      if (map->arr[i][j] != NULL)
      {
        if (map->arr[i][j]->item == '.')
        {
          open_spaces++;
        }
      }
    }
  }

  int max_num_piles;
  if (num_piles > open_spaces - 26)
  {
    max_num_piles = open_spaces - 26; //To allow for 26 players
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
      srand(time(0));
      pile_gold = 1 + (rand() % ((gold_left - piles_left + 1) / 2)); //Divide max by 2 so as to not get disproportionately large piles
    }

    gold_left = gold_left - pile_gold;

    //genreate random gold location
    int gold_location = (rand() % open_spaces);

    //Loop through, and add gold to this location
    int space = 0;
    for (int i = 0; i < 500; i++)
    {
      for (int j = 0; j < 500; j++)
      {
        if (map->arr[i][j] != NULL)
        {
          if (map->arr[i][j]->item == '.')
          {
            if (space == gold_location)
            {
              map->arr[i][j]->item = '*';
              gold_pile_t *pile = new_gold_pile(pile_gold);
              map->arr[i][j]->type = pile;
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

/* WHen there is a message to parse from a client */
bool handleMessage(void *arg, const addr_t from, const char *message)
{


        // Check if the port is null
        addr_t *otherp = (addr_t *)arg;

        // defensive
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
                // check and make sure there are not more than 26 users
                if (game->numplayers >= 26)
                {
                        fprintf(stderr, "Sorry, no more than 26 players can join! Please try again later.\n");
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

                // make sure the name is truncated to 20 char
                if (strlen(real_name) > 20)
                {
                        int diff = strlen(real_name) - 20;
                        real_name[strlen(real_name) - diff] = '\0';
                }

                // Create new player
                player_t *newPlayer = player_new(game->map, real_name);

                // Add the player to the game struct array
                server_dropPlayer(game->map, newPlayer);
                playerNode_t *node = playerNode_new();
                node->address = from;
                node->player = newPlayer;

                // add the player to the array by determing the index to add to
                int i = 0;
                while (((game->players->arr[i]) != NULL) && (i < 26))
                {
                        i++;
                }

                // Assign the player to arr[i] and add the player letter 'A'
                (game->players->arr[i]) = node;
                char *playerLetter;

                log_d("The index: %d\n", i);

                // Add their corresponding char for the spectator and end of game summary
                if (game->players->arr[i] != NULL)
                {
                        log_v("Successfully added a player\n");

                        // Determine their characters
                        playerLetter = getCharacterBasedOnIndex(i);
                        log_s("The character letter %s\n", playerLetter);
                        server_dropPlayer_char(newPlayer, *playerLetter);

                        game->players->arr[i]->playerLetter = playerLetter;
                }

                //Increment the number of players
                game->numplayers++;

                // Respond with OK 'char letter'
                char letter[100] = "OK ";
                strcat(letter, playerLetter);

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
                        char *message_to_send = game_over_summar();
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
                char *message_to_send = game_over_summar();
                message_send(from, message_to_send);

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
        int gold_left = get_gold_left(game->map);
        if (gold_left == 0)
        {
                // if so, print the tabular summary to all of the players
                for (int j = 0; j < game->numplayers; j++)
                {
                        // send all of the players a tabular summary
                        addr_t new_address = game->players->arr[j]->address;
                        char *message_to_send = game_over_summar();
                        message_send(new_address, message_to_send);
                        free(message_to_send);
                }
                // as well as the spectator if applicable
                if (game->spectator != NULL)
                {
                        // send all of the players a tabular summary
                        addr_t new_address = game->spectator->address;
                        char *message_to_send = game_over_summar();
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
                        send_player_gold(game, player, game->players->arr[j]->address);
                        send_player_display(game, player, game->players->arr[j]->address);
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

/* Returns the alphabetical letter based on index */
char *getCharacterBasedOnIndex(int i)
{
    char *toReturn;
    if (i < 0 || i > 25)
    {
        fprintf(stderr, "Please enter a valid index number.\n");
        toReturn = NULL;
    }
    else
    {
        toReturn = (char *)malloc(sizeof(char));
        *toReturn = (char)(i + 65);
    }
    return toReturn;
}

void move_left(map_t *map, player_t *player)
{
  if (player->x >= 1)
  {
    if (moveable(map->arr[player->y][player->x - 1]))
    {
      player_move(map, player, player->x - 1, player->y);
    }
  }
}

void move_left_MAX(map_t *map, player_t *player)
{
  while ((moveable(map->arr[player->y][player->x - 1])) && (player->x >= 1))
  {
    player_move(map, player, player->x - 1, player->y);
  }
}

void move_right(map_t *map, player_t *player)
{
  if (player->x < map->columns)
  {
    if (moveable(map->arr[player->y][player->x + 1]))
    {
      player_move(map, player, player->x + 1, player->y);
    }
  }
}

void move_right_MAX(map_t *map, player_t *player)
{
  while ((moveable(map->arr[player->y][player->x + 1])) && (player->x < map->columns))
  {
    player_move(map, player, player->x + 1, player->y);
  }
}

void move_up(map_t *map, player_t *player)
{
  if (moveable(map->arr[player->y - 1][player->x]))
  {
    player_move(map, player, player->x, player->y - 1);
  }
}

void move_up_MAX(map_t *map, player_t *player)
{
  while (moveable(map->arr[player->y - 1][player->x]))
  {
    player_move(map, player, player->x, player->y - 1);
  }
}

void move_down(map_t *map, player_t *player)
{
  if (moveable(map->arr[player->y + 1][player->x]))
  {
    player_move(map, player, player->x, player->y + 1);
  }
}

void move_down_MAX(map_t *map, player_t *player)
{
  while (moveable(map->arr[player->y + 1][player->x]))
  {
    player_move(map, player, player->x, player->y + 1);
  }
}

void move_diag_up_left(map_t *map, player_t *player)
{
  if (moveable(map->arr[player->y - 1][player->x - 1]))
  {
    player_move(map, player, player->x - 1, player->y - 1);
  }
}

void move_diag_up_left_MAX(map_t *map, player_t *player)
{
  while (moveable(map->arr[player->y - 1][player->x - 1]))
  {
    player_move(map, player, player->x - 1, player->y - 1);
  }
}

void move_diag_up_right(map_t *map, player_t *player)
{
  if (moveable(map->arr[player->y - 1][player->x + 1]))
  {
    player_move(map, player, player->x + 1, player->y - 1);
  }
}

void move_diag_up_right_MAX(map_t *map, player_t *player)
{
  while (moveable(map->arr[player->y - 1][player->x + 1]))
  {
    player_move(map, player, player->x + 1, player->y - 1);
  }
}

void move_diag_down_left(map_t *map, player_t *player)
{
  if (moveable(map->arr[player->y + 1][player->x - 1]))
  {
    player_move(map, player, player->x - 1, player->y + 1);
  }
}

void move_diag_down_left_MAX(map_t *map, player_t *player)
{
  while (moveable(map->arr[player->y + 1][player->x - 1]))
  {
    player_move(map, player, player->x - 1, player->y + 1);
  }
}

void move_diag_down_right(map_t *map, player_t *player)
{
  if (moveable(map->arr[player->y + 1][player->x + 1]))
  {
    player_move(map, player, player->x + 1, player->y + 1);
  }
}

void move_diag_down_right_MAX(map_t *map, player_t *player)
{
  while (moveable(map->arr[player->y + 1][player->x + 1]))
  {
    player_move(map, player, player->x + 1, player->y++);
  }
}

bool moveable(mapnode_t *node)
{
  mapnode_t *this_node = node;
  char c;
  if (this_node != NULL)
  {
    c = this_node->item;
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
  map->arr[player->y][player->x]->item = '@';
  map->arr[player->y][player->x]->type = player;
  make_visible(player, map);
}

int get_num_gold(player_t *player)
{
  return (player->gold);
}

char *get_real_name(player_t *player)
{
  return (player->name);
}

/********************* player_move ********************/
void player_move(map_t *map, player_t *player, int new_x, int new_y)
{
  int x = player_getXPosition;
  int y = player_getYPosition;

  char char_to_switch;  //Character that the player is switching with
  void *item_to_switch; //Type that the character is switching with

  if (map->arr[new_y][new_x]->item == '*')
  { //If a pile of gold
    gold_pile_t *pile = map->arr[new_y][new_x]->type;
    map->goldleft -= pile->amount; //subtract amoun of gold from goldleft
    player->justCollected = pile->amount;
    player->gold += pile->amount;
    char_to_switch = '.';  //Player is switching with empty space
    item_to_switch = NULL; //Player is switching with null type
    //delete_gold_pile(pile);
  }
  else if (map->arr[new_y][new_x]->item == '#')
  { //if going down a hallway
    //same (hallway boolean in player takes care of hallway print)
    char_to_switch = '.';
    item_to_switch = NULL;
  }
  else if (map->arr[new_y][new_x]->item == '@')
  { //if going down a hallway
    player_t *swapped_player = (player_t *)(map->arr[new_y][new_x]->type);
    swapped_player->x = x;
    swapped_player->y = y;
    char_to_switch = map->arr[new_y][new_x]->item; //character at new place
    item_to_switch = map->arr[new_y][new_x]->type; //type at new place
  }
  else
  {
    char_to_switch = map->arr[new_y][new_x]->item; //character at new place
    item_to_switch = map->arr[new_y][new_x]->type; //type at new place
  }

  //set player coords to new spot
  player->x = new_x;
  player->y = new_y;

  //set things at new spot to the '@' character and player
  map->arr[new_y][new_x]->item = '@';
  map->arr[new_y][new_x]->type = player;

  if (map->arr[y][x]->hallway)
  { //If a hallway is under player
    //Keep it that way
    map->arr[y][x]->item = '#';
    map->arr[y][x]->type = NULL;
  }
  else
  { //If not
    //Put the new charcter and item where player is
    map->arr[y][x]->item = char_to_switch;
    map->arr[y][x]->type = item_to_switch;
  }

  make_visible(player, map);
}