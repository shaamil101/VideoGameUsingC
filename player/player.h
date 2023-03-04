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
*  NULL pointer if values provided are NULL
* 
*/
player_t* player_new(char* playerName, addr_t ipAddress, int maxCharacters, int totalRows, int totalCollumns, char letterAssigned);


/** player_delete
*
* frees the elements within a player and the player itself
*
* Caller provides:
*  valid pointer to a player
* We return:
*  nothing
* 
*/
void player_delete(player_t* player);


/** player_addGold
*
* increments players gold amount
*
* Caller provides:
*  valid pointer to player, integer for
*  amount of gold
* We return:
*  nothing
* 
*/
void player_addGold(player_t* player, int gold);


/** player_getGold
*
* returns the players goal amount
*
* Caller provides:
*  valid pointer to player
* We return:
*  integer of the player's amount of
* 
*/
int player_getGold(player_t* player);


/** player_getJustCollected
*
* returns the players just Collected
*
* Caller provides:
*  valid pointer to player
* We return:
*  integer of the player's amount of gold last collected
* 
*/
int player_getJustCollected(player_t* player);

/** player_setJustCollected
*
* sets the just collected value given an integer 
*
* Caller provides:
*  valid pointer to player
* We return:
*  integer of the player's amount of gold last collected
* 
*/
void player_setJustCollected(player_t* player, int justCollected);


/** player_addSeenMap
*
* Adds either true or false to the inner array at the
* correct x (outer array) and correct y (inner array)
*
* Caller provides:
*  valid pointer to player, collumn number, row number, boolean
*  indicating if that point has been seen or not
*
* We return:
*  nothing
* 
*/
void player_addSeenMap(player_t* player, int collumn, int row, bool state);

/** player_getSeenMap
*
* returns the players seenmap (2d array of booleans indicating
* what the player has previously seen)
*
* Caller provides:
*  valid pointer to player
* We return:
*  2d array of booleans indicating
*  what the player has previously seen
* 
*/
bool** player_getSeenMap(player_t* player);


/** player_setXPosition
*
* Sets the players current x position on the map.
* Server module checks if x is in range
*
* Caller provides:
*  valid pointer to player, integer for x position
* We return:
* nothing
* 
*/
void player_setXPosition(player_t* player, int xPos);


/** player_getXPosition
*
* Returns the players current x position on the map 
*
*
* Caller provides:
*  valid pointer to player, integer for x position
* We return:
*  integer for players x position
* 
*/
int player_getXPosition(player_t* player);


/** player_setYPosition
*
* Sets the players current y position on the map.
* Server module checks if y is in range
*
* Caller provides:
*  valid pointer to player, integer for y position
* We return:
* nothing
* 
*/
void player_setYPosition(player_t* player, int yPos);


/** player_getYPosition
*
* Returns the players current y position on the map
*
* Caller provides:
*  valid pointer to player
* We return:
*   integer for players Y position
* 
*/

int player_getYPosition(player_t* player);

/** player_getRealName
*
* Returns the players real name (truncated version less than max length)
*  given at the start of the game)
*
* Caller provides:
*  valid pointer to player
* We return:
*   character pointer to player's realName
* 
*/
char* player_getRealName(player_t* player);

/** player_getLetterAssigned
*
* Returns the players letter assigned
*
* Caller provides:
*  valid pointer to player
* We return:
*   character of the players letter
* 
*/
char player_getLetterAssigned(player_t* player);

/** player_setLetterAssigned
*
* Sets the players letter assigned to the value given 
*
* Caller provides:
*  valid pointer to player and letter assigned to player
* We return:
*  nothing
* 
*/
void player_setLetterAssigned(player_t* player, char letter);

/** player_getIP
*
* Returns the players real name (truncated version less than max length)
*  given at the start of the game)
*
* Caller provides:
*  valid pointer to player
* We return:
*   character pointer to player's realName
* 
*/
addr_t player_getIP(player_t* player);
