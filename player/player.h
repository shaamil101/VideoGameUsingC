/*
* player.h: header file for player.c
*
* Author: Wall Street Djournal 
* Date: 2/28/23
* CS 50, Winter 2023, Nuggets Game
*/


typedef struct player player_t;

player_t* player_new(const char* playerName, const char* ipAddress, int maxCharacters, int totalRows, int totalCollumns);

void player_delete(player_t* player);

void player_addGold(player_t* player, int gold);


