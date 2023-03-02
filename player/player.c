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
#include <player.h>
#include <mem.h>
#include <../support/message.h>
#include <map.h>

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

player_t* player_new(const char* playerName, addr_t ipAddress, int maxCharacters, map_t map, char letterAssigned){
    if (playerName && maxCharacters && map_getRows(map) && map_getCols(map)){ //null check 
        player_t* playerNew = mem_malloc(sizeof(player_t));
        if (strlen(playerName) > maxCharacters){
            char* newName;
            for (int i = 0; i < maxCharacters; i ++){
                newName[i] = playerName[i];
            }
            playerName = newName;
        }
        playerNew->letterAssigned = letterAssigned;
        playerNew->realName = mem_malloc(sizeof(char)*maxCharacters);
        playerNew->realName = playerName;
        playerNew->IP = ipAddress;
        playerNew->gold = 0;
        playerNew->justCollected = 0;
        playerNew->seenMap = mem_malloc((map_getRows(map) * map_getCols(map))*sizeof(bool));
        playerNew->x = NULL; 
        playerNew->y = NULL; 
        return playerNew;
    }
    return NULL;
}

void player_delete(player_t* player){
    if (player){ //null check 
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
    return NULL;
}

int player_getJustCollected(player_t* player){
    if (player){ //null check 
            return player->justCollected;
    }
    return NULL;
}

void player_addSeenMap(player_t* player, int collumn, int row, bool state, map_t map){
    if (player){//null check
        if ((0 <= player->x <= map_getCols(map)) && (0 <= player->y <= map_getRows(map))){
            bool temp = player->seenMap[collumn][row];
            temp = state;
        }
    }
}

bool** player_getSeenMap(player_t* player){
    if (player){
        return player->seenMap;
    }
    return NULL;
}

void player_setXPosition(player_t* player, int xPos, map_t map){
    if (player){
        if (0 <= xPos <= map_getCols(map)){
            player->x = xPos;
        }
    }
    return; 
}

int player_getXPosition(player_t* player){
    if (player){
        return player->x;
    }
    return NULL;
}

void player_setYPosition(player_t* player, int yPos, map_t map){
    if (player){
        if (0 <= yPos <= map_getRows(map)){
            player->y = yPos;
        }
    }
}

int player_getYPosition(player_t* player){
    if (player){
        return player->y;
    }
    return NULL;
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
    return NULL;
}

addr_t player_getIP(player_t* player){
    if (player){
        return player->IP;
    }
    return NULL;
}