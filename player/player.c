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

typedef struct player{
    int x;                        
    int y;                    
    bool** seenMap;
    int gold;
 	char* IP;                     
    char* realName;
} player_t; 

player_t* player_new(const char* playerName, const char* ipAddress, int maxCharacters, int totalRows, int totalCollumns){
    if (playerName && ipAddress && maxCharacters && totalRows && totalCollumns){ //null check 
        player_t* playerNew = mem_malloc(sizeof(player_t));
        if (strlen(playerName) > maxCharacters){
            char* newName;
            for (int i = 0; i < maxCharacters; i ++){
                newName[i] = playerName[i];
            }
            playerName = newName;
        }

        playerNew->realName = mem_malloc(sizeof(char)*maxCharacters);
        playerNew->realName = playerName;
        playerNew->IP = ipAddress;
        playerNew->gold = 0;
        playerNew->seenMap = mem_malloc((totalRows*totalCollumns)*sizeof(bool));
        // if (map){
        //     map_get gridpoint;
        //     playerNew->x = 0; //set x to collumn from gridpoint
        //     playerNew->y = 0; //set y to row from gridpoint
        // }
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
            player->gold = player->gold + gold;
        }
        return;
    }
}

void player_addSeenMap(player_t* player, int row, int collumn){
    if (player){//null check
        
    }
}