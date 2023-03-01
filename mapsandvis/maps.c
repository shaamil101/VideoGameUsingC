/**
 * maps.c - maps module for CS50 nuggets
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
#include <stdio.h>
#include <stdbool.h>
#include "maps.h"
#include "log.h"
#include "mem.h"
#include "file.h"

typedef struct mapNode{
  char item;
  void* type;
} mapNode_t;

typedef struct map{
  mapNode_t*** grid; //2d array of mapNode_t pointers
	int numRows;
	int numCols;
} map_t;

typedef struct matrixIndex {
  int row;
  int col;
} matrixIndex_t;

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
map_t* maps_new(char* mapTextAddress)
{
  log_init(stderr);
  if (mapTextAddress == NULL) { // validate args
    log_v("Text map passed to maps_new is NULL");
    return NULL;
  }
  FILE* fp;
  if ((fp = fopen(mapTextAddress, 'r')) == NULL) { // verify mapTextAddress is a readable file (in requirements we are allowed to assume it is a valid map)
    log_s("Text map %s could not be read", mapTextAddress);
    return NULL;
  }
  map_t* map = mem_malloc_assert(sizeof(map_t), "maps.c: maps_new: could not allocate memory for new map struct");// allocate memory for a map struct, still unkown number of rows and columns for grid
	char* fileline = file_readFile(fp); // read the file into one character buffer, need to free later
  if (fileline = NULL) {
    log_s("Text map %s could not be scanned", mapTextAddress);
  }
  fclose(fp);
  char* scanpointer = fileline; // for scanning through file
  int numrows = 0;
  int numcols = 0;
  while (scanpointer != '\n' || scanpointer != NULL) { // pass through first line to get number of columns
    numcols++;
    scanpointer++;
  }
  while (scanpointer != NULL) { // keep passing through, counting number of new lines = num rows
    if (scanpointer == '\n') {
      numrows++;
    }
    scanpointer++;
  }
	if (numcols == 0 || numrows == 0) { // row/col validity check
    log_s("Scanned text map %s to be an invalid number of rows or columns", mapTextAddress);
    return NULL;
  }
  (*map).numCols = numcols;// set row and column numbers of map struct
  (*map).numRows = numrows;

	mapNode_t* (*grid) [numrows] = malloc( sizeof(mapNode_t*[numrows][numcols]) ); // allocate memory for the 2d char array of the map struct, with number of rows and columns for matrix size
	
  scanpointer = fileline;
  int row_idx = 0;
  int col_idx = 0;
  while (scanpointer != NULL) {// go again through character buffer, keeping track of row and column indices
    if (scanpointer == '\n') {
      row_idx = 0;
      col_idx++;
      scanpointer++;
      continue;
    }
    if (scanpointer != ' ' || scanpointer != '-' || scanpointer != '|' || scanpointer != '+' || scanpointer != '.' || scanpointer != '#') { // 	easy to check right here if char is valid (' ' - | + . # are valid)
     log_s("Text map %s had an invalid character", mapTextAddress); 
    }
    mapNode_t* mapNode = mapNodeNew(scanpointer);
    if (mapNode == NULL) {
      log_v("Expected valid mapNode* but got NULL instead");
      return NULL;
    }
    grid[row_idx][col_idx] = mapNode; // copy into the grid
    scanpointer++;
  }
  map->grid = grid; // set the map grid 

	return map;// return the map struct
}


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

/** maps_fullgrid
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
char* maps_fullgrid(map_t* map, PLAYER_T* playerList, GOLD_T* goldList);

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
char* maps_playergrid(map_t* map, PLAYER_T* player, GOLD_T* goldList);

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

/** maps_getGridpoint
 * 
 * Returns the char at the row, column index of a given map grid
 * 
 * Caller provides
 *  valid map pointer
 *  valid (non-negative) row
 *  valid (non-negative) column
 * We return
 *  char at that gridpoint
 *  Null char if anything invalid
*/
char maps_getGridpoint(map_t* map, int row, int col);

/** maps_getVisiblePoints
 * 
 * Returns a list of visible matrixIndices from a given player's position
 * 
 * Caller provides:
 *  valid map pointer
 *  valid player pointer
 * We return:
 *  null-terminated pointer list of matrixIndex pointers
 *  Null if invalid
 * Caller is responsible for later freeing pointer list
*/
matrixIndex_t* maps_getVisiblePoints(map_t* map, PLAYER_T* player);

/** maps_getRandomGridpoint
 * 
 * Returns a random empty room point on a given map
 * 
 * Caller provides
 *  valid map pointer
 * We return
 *  matrixIndex pointer to an empty-room point for that map
 * Caller is responsible for later freeing matrixIndex pointer
*/
matrixIndex_t* maps_getRandomGridpoint(map_t* map);

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

/** maps_newMatrixIndex
 * 
 * Creates a new matrixIndex struct with a row and col
 * 
 * Caller provides:
 *  int row that is 0 or greater
 *  int col that is 0 or greater
 * We return
 *  matrixIndex pointer
 *  or NULL if memory allocation issues
 * Caller is responsible for later freeing this struct with maps_deleteMatrixIndex
*/
matrixIndex_t* maps_newMatrixIndex(int row, int col);

/** maps_compareMatrixIndex
 * 
 * Compares whether or not two matrix indices are the same
 * 
 * Caller provides:
 *  valid matrixIndex pointer A
 *  valid matrixIndex pointer B
 * We return:
 *  boolean value for if A and B are the same index
 *  (false if invalid args)
 */
bool maps_compareMatrixIndex(matrixIndex_t* indexA, matrixIndex_t* indexB);

/** maps_deleteMatrixIndex
 * 
 * Frees the matrixIndex struct from memory
 * 
 * Caller provides:
 *  valid matrixIndex pointer
 * We guarantee
 *  the matrixIndex is freed from memory
*/
void maps_deleteMatrixIndex(matrixIndex_t* index);

/** mapNodeNew
 * 
 * Creates a new mapNode structure and returns its pointer
*/
mapNode_t* mapNodeNew(char item)
{
  // fine if item is NULL, no need to check
  mapNode_t* node = mem_malloc_assert(sizeof(mapNode_t), "Unable to allocate memory for mapnode struct\n");
  node->item = item;
  return node;
}

void mapNodeDelete(mapNode_t* node)
{
  if (node == NULL) {
    log_v("mapNodeDelete: tried to delete NULL node!");
    return;
  }
  free(node);
  // don't delete item because it's either a gold pile or a player, server will free those
}