#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "support/message.h"
#include "support/log.h"
#include <ctype.h>
#include <unistd.h>
#include "memory.h"

/**************** global types ****************/
typedef struct game game_t;

/**************** local types ****************/
typedef struct playerTable playerTable_t;


void server_dropGold(map_t *map, int num_piles, int gold_amount);
bool handleMessage(void *arg, const addr_t from, const char *message);
gold_pile_t *new_gold_pile(int size);
void delete_gold_pile(gold_pile_t *pile);
void playerTable_delete(playerTable_t *playerTable);
spec_t *spectator_new(addr_t address);
playerTable_t *playerTable_new();
game_t *gamenode_new(int num_nuggets, map_t *curr_map);
void send_player_display(game_t *game, player_t *player, addr_t from);
void send_player_gold(game_t *game, player_t *player, addr_t from);
void send_spectator_gold(game_t *game, addr_t from);
char *createGoldMessage(map_t *map, player_t *player);
player_t *player_set(map_t *map, player_t *player);
char *getCharacterBasedOnIndex(int i);
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
bool moveable(mapnode_t *node);
void server_dropPlayer(map_t *map, player_t *player);
void player_move(map_t *map, player_t *player, int new_x, int new_y);
char *game_over_summary();
player_t *searchByAddress(addr_t from);
bool isNumber(char number[]);
void free_everything(game_t *game);
void make_visible(player_t *player, map_t *map);
void send_spectator_display(game_t *game, addr_t from);