/*
* player.h: header file for player.c
*
* Author: Wall Street Djournal 
* Date: 2/28/23
* CS 50, Winter 2023, Nuggets Game
*/

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "mem.h"
#include "message.h"



typedef struct player player_t;
/** player_new
 * 
 * Returns a pointer to a player
 * 
 * Caller provides:
 *  valid player name, ipaddress, integer for max characters allowed in name
 *  total rows and total collumns, and the letter assigned to the player
 * We return:
 *  Returns a pointer to a player with values instantiated 
 *  NULL pointer if given node is NULL
 *  Null
*/

/** player_new
 * 
 * Returns a pointer to a player
 * 
 * Caller provides:
 *  valid player name, ipaddress, integer for max characters allowed in name
 *  total rows and total collumns, and the letter assigned to the player
 * We return:
 *  Returns a pointer to a player with values instantiated 
 *  NULL pointer if given node is NULL
 *  Null
*/
player_t* player_new(char* playerName, addr_t ipAddress, int maxCharacters, int totalRows, int totalCollumns, char letterAssigned);

void player_delete(player_t* player);

void player_addGold(player_t* player, int gold);

int player_getGold(player_t* player);

int player_getJustCollected(player_t* player);

void player_setJustCollected(player_t* player, int justCollected);

void player_addSeenMap(player_t* player, int collumn, int row, bool state);

bool** player_getSeenMap(player_t* player);

void player_setXPosition(player_t* player, int xPos);

int player_getXPosition(player_t* player);

void player_setYPosition(player_t* player, int yPos);

int player_getYPosition(player_t* player);

char* player_getRealName(player_t* player);

char player_getLetterAssigned(player_t* player);

addr_t player_getIP(player_t* player);