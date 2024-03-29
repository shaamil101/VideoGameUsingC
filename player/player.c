/*
* player.c: defines player struct and a set of methods (see player.h for more details)
*
* Author: Wall Street Djournal 
* Date: 2/28/23
* CS 50, Winter 2023, Nuggets Game
*/

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "player.h"
#include "mem.h"
#include "message.h"

typedef struct player{
    int x;                        
    int y;                    
    bool** seenMap;
    int gold;
    int justCollected;
 	addr_t IP;      
    char letterAssigned;               
    char* realName;
} player_t; 
/**player_new
*
* see player.h for more details
*/
player_t* player_new(char* playerName, addr_t ipAddress, int maxCharacters, int xRange, int yRange, char letterAssigned){
    if (playerName && maxCharacters){ //null check 
        player_t* playerNew = mem_malloc(sizeof(player_t));
        char* name;
        if (strlen(playerName) > maxCharacters){
            name = mem_calloc_assert(maxCharacters+1,sizeof(char),"Unable to allocate memory for playername");
            for (int i = 0; i < maxCharacters; i ++){
                name[i] = playerName[i];
            }
        } else {
            name = mem_calloc_assert(maxCharacters+1,sizeof(char),"Unable to allocate memory for playername");
            strcpy(name, playerName);
        }
        playerNew->realName = name;
        playerNew->letterAssigned = letterAssigned;
        playerNew->IP = ipAddress;
        playerNew->gold = 0;
        playerNew->justCollected = 0;

        bool** seenMap = mem_calloc_assert(yRange,sizeof(bool*), "unable to allocate memory for player->seenMap");
        for (int i = 0; i < yRange; i++) {
            seenMap[i] = mem_calloc_assert(xRange,sizeof(bool), "unable to allocate memory for player->seenMap");
            for (int j = 0; j < xRange; j++) {
                seenMap[i][j] = false;
            }
        }
        playerNew->seenMap = seenMap;
        playerNew->x = 0; 
        playerNew->y = 0; 
        return playerNew;
    }
    return NULL;
}

void player_delete(player_t* player, int yRange){
    if (player){ //null check 
        for (int i = 0; i < yRange; i++) {
            mem_free(player->seenMap[i]);
        }
        mem_free(player->seenMap); //frees 2d array
        mem_free(player->realName); //frees char string 
        mem_free(player); //frees player object 
    }
    return;
}

void player_addGold(player_t* player, int gold){
    if (player){ //null check 
        if (gold > 0){
            player->justCollected = gold;
            player->gold = player->gold + gold;
        }
        return;
    }
}

int player_getGold(player_t* player){
    if (player){ //null check 
            return player->gold;
    }
    return -1;
}

int player_getJustCollected(player_t* player){
    if (player){ //null check 
            return player->justCollected;
    }
    return -1;
}

void player_setJustCollected(player_t* player, int justCollected)
{
    if (player != NULL && justCollected >= 0) {
        player->justCollected = justCollected;
    }
    return;
}

void player_addSeenMap(player_t* player, int x, int y, bool state){
    int row = y;
    int collumn = x;

    if (player){//null check
        player->seenMap[row][collumn] = state;
    }
}

bool** player_getSeenMap(player_t* player){
    if (player){
        return player->seenMap;
    }
    return NULL;
}

void player_setXPosition(player_t* player, int xPos){
    if (player){
        player->x = xPos;
    }
    return; 
}

int player_getXPosition(player_t* player){
    if (player){
        return player->x;
    }
    return -1;
}

void player_setYPosition(player_t* player, int yPos){
    if (player){
        player->y = yPos;
    }
}

int player_getYPosition(player_t* player){
    if (player){
        return player->y;
    }
    return -1;
}

char* player_getRealName(player_t* player){
    if (player){
        return player->realName;
    }
    return NULL;
}

char player_getLetterAssigned(player_t* player){
    if (player){
        return player->letterAssigned;
    }
    return '\0';
}

void player_setLetterAssigned(player_t* player, char letter)
{
    if (player != NULL && isupper(letter)){
        player->letterAssigned = letter;
    }
    return;
}

addr_t player_getIP(player_t* player){
    if (player){
        return player->IP;
    }
    return message_noAddr();
}