/**
 * maps.h - maps module for CS50 nuggets
 * Wall Street Djournal - CS50 23W
 * 
 * The maps / visibility module provides a map data 
 * structure and a mapNode data structure and 
 * methods to load and validate maps and overlay with 
 * players and gold.
 * 
 * The map struct will have a 2D-array 
 * of mapNodes, and have integers for how tall (number 
 * of rows) and wide (number of columns) it is. 
 * 
 * The matrixNode structure holds info about the
 * room character at that gridpoint, the type of point in 
 * the node (player, gold, base map), and booleans for 
 * whether or not it's transparent and a hallway.
 * 
*/

#include <stdlib.h>
#include "player.h"

/** mapNode_t - opaque to module users
 * 
 * More info in maps.h
 * 
 * Contains information about a specific gridpoint on a map,
 * including the item at that gridpoint (empty room, hallway, player, etc.)
*/
typedef struct mapNode mapNode_t;

/** map_t - opaque to module users 
 * 
 * More info in maps.h
 * 
 * Contains the 2d array of mapNode_t pointeres
 * The number of rows, number of columns, and amount of gold left in the map
*/
typedef struct map map_t;


/** maps_new 
 * 
 * Takes in a mapTextAddress (the char* filepath to a map.txt file),
 * allocates memory for a new map struct
 * verifies and then reads the file and parses
 * it into a 2d array of mapNode structs.
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

/** maps_spectatorgrid
 * 
 * Renders the map into the ascii string to pass to the spectator client
 * with all gold and all players in it
 * 
 *Caller provides:
 *  valid map pointer
 *  valid list of player pointers
 *  valid list of gold pointers
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
 *  valid list of player pointers
 *  valid list of gold pointers
 * We return: a string that's the ascii-rendered map with each row
 *  separated by new line, and null terminated for player-visible overlaid grid
 *  Null if invalid map
 * Caller is responsible for later freeing returned char* memory 
 * 
*/
char* maps_playergrid(map_t* map, player_t* player);

/** maps_isVisible
 * 
 * Returns a boolean value of whether or not a position is visible at another position
 * 
 * Goes through 13 cases:
 * same point as player
 * vertical line: up or down
 * horizontal line: left or right
 * perfectly diagonal line: up-right, up-left, down-right, down-left
 * sloped line: first and eigth octant (of coordinate quadrants), second and third, fourth and fifth, sixth and seventh octants
 * 
 * Calls the static octant functions for keeping code a bit tidier, but the straight and diagonal line cases are handled in this method
 * 
 * Sees if visible for if whether or not a ray from the player to the test point is blocked by an opaque point - if in between two blocks,
 * cannot see through if both blocks are opaque
 * 
 * Caller provides:
 *  a valid int for player row 
 *    and column
 *  a valid int for test row
 *    and column
 * We return
 *  bool for whether it's visible at that
*/
bool maps_isVisible(map_t* map, int playerX, int playerY, int testX, int testY);

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

/** maps_getXrange
 * 
 * Returns the integer max x value in the grid (number of columns)
 * 
 * Caller provides:
 *  valid map pointer
 * We return: integer number of max x value in grid
 * Or 0 if invalid map struct
*/
int maps_getXrange(map_t* map);

/** maps_getYrange
 * 
 * Returns the integer max Y value in the grid (number of rows)
 * 
 * Caller provides:
 *  valid map pointer
 * We return: integer number of max Y value in grid
 * Or 0 if invalid map struct
*/
int maps_getYrange(map_t* map);

/** maps_getMapNode
 * 
 * Returns the mapNode at the x,y index of a given map grid
 * 
 * Caller provides
 *  valid map pointer
 *  valid (non-negative) x
 *  valid (non-negative) y
 * We return
 *  mapNode pointer at that gridpoint
 *  Null pointer if anything invalid
*/
mapNode_t* maps_getMapNode(map_t* map, int x, int y);

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
 * We guarantee:
 *  if the node is not null, to set the node->item to the given char
*/
void maps_setMapNodeItem(mapNode_t* node, char item);

/** maps_getMapNodeType
 * 
 * Sets the void* type for a given mapNode
 * 
 * Caller provides:
 *  valid mapNode pointer
 *  valid void* type in the mapNode (player or gold)
 * We guarantee:
 *  if the node isn't null, to set the void* type pointer of the node to the given void*
*/
void maps_setMapNodeType(mapNode_t* node, void* type);

/** maps_setTotalGoldLeft
 * 
 * Sets the integer total gold left in a map
 * 
 * Caller provides:
 *  valid map_t pointer
 *  int totalGoldLeft (>0) to set to
 * We guarantee:
 *  to set the map->totalGoldLeft to totalGoldLeft
 *  or to set it to 0 if given a negative value
 *  or to do nothing if map is null
*/
void maps_setTotalGoldLeft(map_t* map, int totalGoldLeft);

/** maps_getTotalGoldLeft
 * 
 * Gets the integer total gold left in a map
 * 
 * Caller provides:
 *  valid map_t pointer
 * We return:
 *  integer value for amount of gold left in map
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


/** maps_isHallwayNode
 * 
 * Returns whether or not a mapNode is a hallway point
 * 
 * Caller provides:
 *  valid mapNode_t pointer
 * We return:
 *  if the gridpoint is a hallway (originally a '#')
*/
bool maps_isHallwayNode(mapNode_t* node);