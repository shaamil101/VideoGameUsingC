/**
 * maps.h - maps module for CS50 nuggets
 * 
 * Wall Street Djournal - CS50 23W
 * 
 * The maps / visibility module provides a map data 
 * structure and a matrixIndex data structure and 
 * methods to load and validate maps and overlay with 
 * players and gold. The map will be stored as a 2D-array 
 * of characters, and have integers for how tall (number 
 * of rows) and wide (number of columns) it is. 
 * The matrixIndex structure just wraps two integers 
 * for a row and a column index into a 2D array.
 * 
*/

#include <stdlib.h>
#include "player.h"

typedef struct map map_t;

typedef struct mapNode mapNode_t;

/** maps_new 
 * 
 * Takes in a mapTextAddress (the char* filepath to a map.txt file),
 * allocates memory for a new map struct
 * verifies and then reads the file and parses
 * it into a 2d array of characters.
 * Memory allocated for this must later be freed
 * with `maps_delete`
 * 
 * Caller provides:
 *  char* mapTextAddress, a string filepath for a valid map text file
 * We return:
 *  a valid map struct pointer if mapTextAddress was valid, NULL otherwise
 * Caller is responsible for later freeing map struct with maps_delete
 */
map_t* maps_new(char* mapTextAddress);


/** maps_basegrid
 * 
 * Renders the map into the ascii string
 * without any gold or players in it
 * 
 * Caller provides:
 *  valid map pointer
 * We return: a string that's the ascii-rendered map with each row
 *  separated by new line, and null terminated
 *  Null if invalid map
 * Caller is responsible for later freeing returned char* memory
*/
char* maps_basegrid(map_t* map);

char* maps_visiblebasegrid(map_t* map, int row, int col);

/** maps_spectatorgrid
 * 
 * Renders the map into the ascii string to pass to the spectator client
 * with all gold and all players in it
 * 
 *Caller provides:
 *  valid map pointer
 * We return: a string that's the ascii-rendered map with each row
 *  separated by new line, and null terminated for entire overlaid grid
 *  Null if invalid map
 * Caller is responsible for later freeing returned char* memory 
 * 
*/
char* maps_spectatorgrid(map_t* map);

/** maps_playergrid
 * 
 * Renders the map into the ascii string to pass to the player client
 * with gold and players but only points seen by a given player
 * 
 *Caller provides:
 *  valid map pointer
 *  valid player pointer
 * We return: a string that's the ascii-rendered map with each row
 *  separated by new line, and null terminated for player-visible overlaid grid
 *  Null if invalid map
 * Caller is responsible for later freeing returned char* memory 
 * 
*/
char* maps_playergrid(map_t* map, player_t* player);

/** maps_getRows
 * 
 * Returns the integer number of rows in a map struct's grid
 * 
 * Caller provides:
 *  valid map pointer
 * We return: integer number of rows in grid
 * Or 0 if invalid map struct
*/
int maps_getRows(map_t* map);

/** maps_getCols
 * 
 * Returns the integer number of columns in a map struct's grid
 * 
 * Caller provides:
 *  valid map pointer
 * We return: integer number of columns in grid
 * Or 0 if invalid map struct
*/
int maps_getCols(map_t* map);

int maps_getXrange(map_t* map);

int maps_getYrange(map_t* map);

/** maps_getMapNode
 * 
 * Returns the mapNode at the row, column index of a given map grid
 * 
 * Caller provides
 *  valid map pointer
 *  valid (non-negative) row
 *  valid (non-negative) column
 * We return
 *  mapNode pointer at that gridpoint
 *  Null pointer if anything invalid
*/
mapNode_t* maps_getMapNode(map_t* map, int row, int col);

/** maps_getMapNodeItem
 * 
 * Returns the char item for a given mapNode
 * 
 * Caller provides:
 *  valid mapNode pointer
 * We return:
 *  char in the mapNode
 *  NULL char if given node is NULL
*/
char maps_getMapNodeItem(mapNode_t* node);

/** maps_getMapNodeType
 * 
 * Returns the void* type for a given mapNode
 * 
 * Caller provides:
 *  valid mapNode pointer
 * We return:
 *  void* type in the mapNode
 *  NULL pointer if given node is NULL
*/
void* maps_getMapNodeType(mapNode_t* node);

/** maps_setMapNodeItem
 * 
 * Sets the char item for a given mapNode
 * 
 * Caller provides:
 *  valid mapNode pointer
 *  char in the mapNode
*/
void maps_setMapNodeItem(mapNode_t* node, char item);

/** maps_getMapNodeType
 * 
 * Returns the void* type for a given mapNode
 * 
 * Caller provides:
 *  valid mapNode pointer
 *  valid void* type in the mapNode (player or gold)
*/
void maps_setMapNodeType(mapNode_t* node, void* type);

/** maps_isVisible
 * 
 * Returns a boolean value of whether or not a position is visible at another position
 * 
 * Caller provides:
 *  a valid matrixIndex pointer for player position
 *  a valid matrixIndex pointer for test position
 * We return
 *  bool for whether it's visible at that
*/
bool isVisible(map_t* map, int playerRow, int playerCol, int testRow, int testCol);

/** maps_setTotalGoldLeft
 * 
 * Sets the integer total gold left in a map
*/
void maps_setTotalGoldLeft(map_t* map, int totalgoldleft);

/** maps_getTotalGoldLeft
 * 
 * Gets the integer total gold left in a map
*/
int maps_getTotalGoldLeft(map_t* map);

/** maps_delete
 * 
 * Frees a given map from memory
 * 
 * Caller provides
 *  a valid map pointer
 * We guarantee:
 *  the map struct and all underlying structs are freed from memory
*/
void maps_delete(map_t* map);

/** mapNodeNew
 * 
 * Creates a new mapNode structure and returns its pointer
*/
mapNode_t* mapNodeNew(char item);

void mapNodeDelete(mapNode_t* node);