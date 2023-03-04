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
#include <ctype.h>
#include <math.h>
#include "maps.h"
#include "log.h"
#include "mem.h"
#include "file.h"
#include "player.h"

typedef struct mapNode{
  char item;
  void* type;
  bool isTransparent;
  bool isHallway;
} mapNode_t;

typedef struct map{
	int numRows;
	int numCols;
  int totalGoldLeft;
  mapNode_t*** grid; //2d array of mapNode_t pointers
} map_t;

static bool firstAndEighthOctant(map_t* map, int playerRow, int playerCol, int testRow, int testCol);
static bool secondAndThirdOctant(map_t* map, int playerRow, int playerCol, int testRow, int testCol);
static bool fourthAndFifthOctant(map_t* map, int playerRow, int playerCol, int testRow, int testCol);
static bool sixthAndSeventhOctant(map_t* map, int playerRow, int playerCol, int testRow, int testCol);

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
  log_v("initialize map logging");
  if (mapTextAddress == NULL) { // validate args
    log_v("Text map passed to maps_new is NULL");
    return NULL;
  }
  log_s("Creating new map from text map %s...", mapTextAddress);
  FILE* fp;
  if ((fp = fopen(mapTextAddress, "r")) == NULL) { // verify mapTextAddress is a readable file (in requirements we are allowed to assume it is a valid map)
    log_s("Text map %s could not be read", mapTextAddress);
    return NULL;
  }
  map_t* map = mem_malloc_assert(sizeof(map_t), "maps.c: maps_new: could not allocate memory for new map struct");// allocate memory for a map struct, still unkown number of rows and columns for grid
	char* fileline = file_readFile(fp); // read the file into one character buffer, need to free later
  if (fileline == NULL) {
    log_s("Text map %s could not be scanned", mapTextAddress);
  }
  fclose(fp);
  char* scanpointer = fileline; // for scanning through file
  int numrows = 0;
  int numcols = 0;
  while (*scanpointer != '\n' && *scanpointer != '\0') { // pass through first line to get number of columns
    numcols++;
    scanpointer++;
  }
  while (*scanpointer != '\0') { // keep passing through, counting number of new lines = num rows
    if (*scanpointer == '\n') {
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

  mapNode_t*** grid = mem_malloc_assert(sizeof(mapNode_t**) * numrows , "maps_new: Unable to allocate memory for grid rows"); // allocate memory for the 2d char array of the map struct, with number of rows and columns for matrix size
	for (int i = 0; i < numrows; i++) {
    grid[i] = mem_malloc_assert(sizeof(mapNode_t*) * numcols, "maps_new: Unable to allocate memory for grid columns");
  }

  scanpointer = fileline;
  int row_idx = 0;
  int col_idx = 0;
  while (*scanpointer != '\0') {// go again through character buffer, keeping track of row and column indices
    if (*scanpointer == '\n') {
      row_idx++;
      col_idx = 0;
      scanpointer++;
      continue;
    }
    if (*scanpointer != ' ' && *scanpointer != '-' && *scanpointer != '|' && *scanpointer != '+' && *scanpointer != '.' && *scanpointer != '#') { // 	easy to check right here if char is valid (' ' - | + . # are valid)
     log_c("Text map had an invalid character %c", *scanpointer); 
    }
    mapNode_t* mapNode = mapNodeNew(*scanpointer);
    if (mapNode == NULL) {
      log_v("Expected valid mapNode* but got NULL instead");
      return NULL;
    }
    grid[row_idx][col_idx++] = mapNode; // copy into the grid
    scanpointer++;
  }
  free(fileline);
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
char* maps_basegrid(map_t* map)
{
  if (map == NULL) {// validate args
    log_v("Got NULL map pointer in maps_basegrid");
    return NULL;
  }
  int numrows = maps_getRows(map);
  int numcols = maps_getCols(map);
	char* mapstring = mem_calloc_assert((numrows+1)*(numcols+1)+1, sizeof(char), "Unable to allocate memory for map string"); // allocate memory for string holding grid
	char* charptr = mapstring;
  for (int r = 0; r < numrows; r++) { // for each row r in the map (starting from 0)
    for (int c = 0; c < numcols; c++) { // 	for each column c in the map (starting from 0)
      *(charptr++) = maps_getMapNodeItem(maps_getMapNode(map, c, r)); // 	add char at that index of the map_t->grid 2d array to the string
    }
    *(charptr++) = '\n';// 	add a new line to the string (for next row)
  }
	*(charptr++) = '\0'; // make last char a terminating null
	return mapstring; // return the string
}

char* maps_visiblebasegrid(map_t* map, int row, int col)
{
  if (map == NULL) {// validate args
    log_v("Got NULL map pointer in maps_basegrid");
    return NULL;
  }
  int numrows = maps_getRows(map);
  int numcols = maps_getCols(map);
	char* mapstring = mem_calloc_assert((numrows+1)*(numcols+1)+1, sizeof(char), "Unable to allocate memory for map string"); // allocate memory for string holding grid
	char* charptr = mapstring;
  for (int r = 0; r < numrows; r++) { // for each row r in the map (starting from 0)
    for (int c = 0; c < numcols; c++) { // 	for each column c in the map (starting from 0)
      if (row == r && col == c) {
        *(charptr++) = '@';
      } else if (maps_isVisible(map, col, row, c, r)) {
        *(charptr++) = maps_getMapNodeItem(maps_getMapNode(map, c, r)); // 	add char at that index of the map_t->grid 2d array to the string
      } else {
        *(charptr++) = ' '; 
      }
    }
    *(charptr++) = '\n';// 	add a new line to the string (for next row)
  }
	*(charptr++) = '\0'; // make last char a terminating null
	return mapstring; // return the string
}

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
char* maps_spectatorgrid(map_t* map)
{
  if (map == NULL) {// validate args
    log_v("Got NULL map pointer in maps_basegrid");
    return NULL;
  }
  int numrows = maps_getRows(map);
  int numcols = maps_getCols(map);
	char* mapstring = mem_calloc_assert((numrows+1)*(numcols+1)+1, sizeof(char), "Unable to allocate memory for map string"); // allocate memory for string holding grid
	char* charptr = mapstring;
  for (int r = 0; r < numrows; r++) {// for each row r in the map (starting from 0)
    for (int c = 0; c < numcols; c++) {// 	for each column c in the map (starting from 0)
      mapNode_t* node = (maps_getMapNode(map, c, r));
      if (node->item == '@') { // if is a player
        player_t* player = (player_t*)(map->grid[r][c]->type);
        *(charptr++) = player_getLetterAssigned(player);
      } else if (node->item == '*') { // if it's gold
        *(charptr++) = node->item;
      } else { // otherwise print normal
        *(charptr++) = maps_getMapNodeItem(maps_getMapNode(map, c, r)); // 	add char at that index of the map_t->grid 2d array to the string
      }
    }
    *(charptr++) = '\n';// 	add a new line to the string (for next row)
  }
	*(charptr++) = '\0'; // make last char a terminating null
	return mapstring; // return the string
}

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
char* maps_playergrid(map_t* map, player_t* player)
{
  if (map == NULL) {// validate args
    log_v("Got NULL map pointer in maps_playergrid");
    return NULL;
  } else if (player == NULL) {
    log_v("Got NULL player pointer in maps_playergrid");
    return NULL;
  }
  int numrows = maps_getRows(map);
  int numcols = maps_getCols(map);
	char* mapstring = mem_calloc_assert((numrows)*(numcols+1)+1, sizeof(char), "Unable to allocate memory for map string"); // allocate memory for string holding grid
	char* charptr = mapstring;
  bool** seenMap = player_getSeenMap(player);
  for (int r = 0; r < numrows; r++) {// for each row r in the map (starting from 0)
    for (int c = 0; c < numcols; c++) {// 	for each column c in the map (starting from 0)
      if (seenMap[r][c]) { // if the player has seen the point before
        mapNode_t* node = (maps_getMapNode(map, c, r));
        if (node != NULL) { // NULL check for node
          if (maps_isVisible(map, player_getXPosition(player), player_getYPosition(player), c, r)) { // if currently visible to player
            if (node->item == '@') { // if is a player
              player_t* playerAtPoint = (player_t*)(map->grid[r][c]->type);
              if (player_getLetterAssigned(playerAtPoint) == player_getLetterAssigned(player)) {
                *(charptr++) = '@'; // if it's the own player
              } else {
                *(charptr++) = player_getLetterAssigned(playerAtPoint); // if it's another player
              }
            } else if (node->item == '*') { // if it's gold
              *(charptr++) = node->item;
            } else { // otherwise print normal
              *(charptr++) = maps_getMapNodeItem(node); // 	add char at that index of the map_t->grid 2d array to the string
            }
          } else { // not currently visible to player
            if (node->item == '@' || node->item == '*') {// if it is a player or gold
              if (maps_isHallwayNode(node)) { // if the player is in a hallway
                *(charptr++) = '#'; // rest to hallway
              } else {
                *(charptr++) = '.'; // make it an empty room slot instead
              }
            } else { // not player or gold and not visible (but already seen by player), add normally
              *(charptr++) = maps_getMapNodeItem(node); // 	add char at that index of the map_t->grid 2d array to the string
            }
          }
        } else {
          log_v("maps_playergrid: got NULL mapNode in the map");
          *(charptr++) = ' ';
        }
      } else { // player hasn't seen the point, render as emtpy
        *(charptr++) = ' ';
      }
    }
    *(charptr++) = '\n';// 	add a new line to the string (for next row)
  }
	*(charptr++) = '\0'; // make last char a terminating null
	return mapstring; // return the string
}

/** maps_isVisible
 * 
 * Returns a boolean value of whether or not a position is visible at another position
 * 
 * Caller provides:
 *  a valid int for player row 
 *    and column
 *  a valid int for test row
 *    and column
 * We return
 *  bool for whether it's visible at that
*/
bool maps_isVisible(map_t* map, int playerX, int playerY, int testX, int testY)
{
  if (map == NULL) { // validate args
    log_v("maps_isVisible: received NULL map pointer");
    return false;
  }
  int playerRow = playerY;
  int playerCol = playerX;
  int testRow = testY;
  int testCol = testX;
  int numRows = map->numRows;
  int numCols = map->numCols;
  if (playerRow < 0 || playerRow >= numRows) { // 	make sure both points row and column values are equal to or less than the map row and column (and non-negative)
    log_d("maps_isVisible: playerRow %d is out of bounds", playerRow);
    return false;
  } else if (playerCol < 0 || playerCol >= numCols) {
    log_d("maps_isVisible: playerCol %d is out of bounds", playerCol);
    return false;
  } else if (testRow < 0 || testRow >= numRows) {
    log_d("maps_isVisible: testRow %d is out of bounds", testRow);
    return false;
  } else if (testCol < 0 || testCol >= numCols) {
    log_d("maps_isVisible: testCol %d is out of bounds", testCol);
    return false;
  }

	// switch through 6 cases: vertical down line (change in columns is 0, change in rows is positive), vertical up line (change in columsn is 0, change in rows is negative), horizontal right line (change in rows is 0, change in columns is positive), horizontal left line (change in rows is 0, change in columns is negative), sloped right line (non-zero change in rows and columns, but change in columns is positive), sloped left line case (non-zero change in rows and columns, but change in columns is negative)
	
  int changeInCols = testCol - playerCol;
  int changeInRows = testRow - playerRow;

  if (changeInCols == 0 && changeInRows == 0) {
    return true; // self is visible
  } else if (changeInCols == 0 && changeInRows > 0) { // vertical down line case: (change in columns is 0, change in rows is positive)
    for (int row = playerRow+1; row < testRow; row++) { 	// 	for each gridpoint along the row (player position + 1 incremented by (1 to testpoint]
      if (!map->grid[row][playerCol]->isTransparent) { // if point isn't transparent
        return false;
      }
    }
    return true; // if we made it this far then it's visible from the player point
  } 
  else if (changeInCols == 0 && changeInRows < 0) { //vertical up line (change in columsn is 0, change in rows is negative)
     for (int row = playerRow-1; row > testRow; row--) { // for each gridpoint along the row ( player position - 1 decremented by 1)
      if (!map->grid[row][playerCol]->isTransparent) { // if point isn't transparent
        return false;
      }
    }
    return true; // return true if we make it to the end 
  } else if (changeInRows == 0 && changeInCols > 0) { // horizontal right line (change in rows is 0, change in columns is positive)
    for (int col = playerCol+1; col < testCol; col++) { // for each gridpoint along the col ( player position + 1 incremented by 1)
      if (!map->grid[playerRow][col]->isTransparent) { // if point isn't transparent
        return false;
      }
    }
    return true; // return true if we make it to the end
  } else if (changeInRows == 0 && changeInCols < 0) { // horizontal left line (change in rows is 0, change in columns is negative)
    for (int col = playerCol-1; col > testCol; col--) { // for each gridpoint along the col ( player position - 1 decremented by 1)
      if (!map->grid[playerRow][col]->isTransparent) { // if point isn't transparent
        return false;
      }
    }
    return true; // return true if we make it to the end
  } else if (changeInRows == changeInCols) {// diagonals
    if (changeInRows > 0) {
      int col = playerCol+1;
      int row = playerRow+1;
      while (col < testCol) {
        if (!map->grid[row++][col++]->isTransparent) { // if point isn't transparent
          return false;
        }
      }
    return true; // return true if we make it to the end
    } else {// changeInRows < 0
      int col = playerCol-1;
      int row = playerRow-1;
      while (col > testCol) {
        if (!map->grid[row--][col--]->isTransparent) { // if point isn't transparent
          return false;
        }
      }
    return true; // return true if we make it to the end
    }
  } else if (changeInRows == -1*changeInCols) { // other diagonals
    if (changeInRows > 0) {
      int col = playerCol-1;
      int row = playerRow+1;
      while (col > testCol) {
        if (!map->grid[row++][col--]->isTransparent) { // if point isn't transparent
          return false;
        }
      }
    return true; // return true if we make it to the end
    } else {// changeInRows < 0
      int col = playerCol+1;
      int row = playerRow-1;
      while (col < testCol) {
        if (!map->grid[row--][col++]->isTransparent) { // if point isn't transparent
          return false;
        }
      }
      return true; // return true if we make it to the end
    }
  }
  else if (changeInRows < 0) {
    if (changeInCols < 0) { // second quadrant
      if (changeInRows < changeInCols) { // third octant
        return secondAndThirdOctant(map, playerRow, playerCol, testRow, testCol);
      } else { // fourth octant
        return fourthAndFifthOctant(map, playerRow, playerCol, testRow, testCol);
      }
    } else if (changeInCols > 0) { // first quadrant
      if (changeInRows < -1*changeInCols) { //second octant
        return secondAndThirdOctant(map, playerRow, playerCol, testRow, testCol);
      } else { // first octant
        return firstAndEighthOctant(map, playerRow, playerCol, testRow, testCol);
      }
    }
  } else if (changeInRows > 0){ // third quadrant
    if (changeInCols < 0) {
      if (changeInRows > -1*changeInCols) { // sixth octant
        return sixthAndSeventhOctant(map, playerRow, playerCol, testRow, testCol);
      } else { // fifth octant
        return fourthAndFifthOctant(map, playerRow, playerCol, testRow, testCol);
      }
    } else if (changeInCols > 0) { // fourth quadrant
      if (changeInRows > changeInCols) { // seventh octant
        return sixthAndSeventhOctant(map, playerRow, playerCol, testRow, testCol);
      } else { // eigth octant
        return firstAndEighthOctant(map, playerRow, playerCol, testRow, testCol);
      }
    }
  }
  return false;
}


/** maps_getRows
 * 
 * Returns the integer number of rows in a map struct's grid
 * 
 * Caller provides:
 *  valid map pointer
 * We return: integer number of rows in grid
 * Or 0 if invalid map struct
*/
int maps_getRows(map_t* map)
{
  if (map == NULL) {
    log_v("map_getRows: map is NULL");
    return 0;
  }
  return map->numRows;
}

/** maps_getCols
 * 
 * Returns the integer number of columns in a map struct's grid
 * 
 * Caller provides:
 *  valid map pointer
 * We return: integer number of columns in grid
 * Or 0 if invalid map struct
*/
int maps_getCols(map_t* map)
{
  if (map == NULL) {
    log_v("map_getCols: map is NULL");
    return 0;
  }
  return map->numCols;
}

int maps_getXrange(map_t* map)
{
  return(maps_getCols(map));
}

int maps_getYrange(map_t* map)
{
  return(maps_getRows(map));
}

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
mapNode_t* maps_getMapNode(map_t* map, int x, int y)
{
  int row = y;
  int col = x;
  if (map == NULL) {
    log_v("map_getMapNode: map is NULL");
    return NULL;
  } else if (row < 0 || row >= map->numRows) {
    log_d("map_getMapNode: row # %d is invalid (must be >=0 and less than map numRows)", row);
    return NULL;
  } else if (col < 0 || col >= map->numCols) {
    log_d("map_getMapNode: col # %d is invalid (must be >=0 and less than map numCols)", col);
    return NULL;
  }
  return map->grid[row][col];
}

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
char maps_getMapNodeItem(mapNode_t* node)
{
  if (node == NULL) {
    log_v("map_getMapNodeItem: mapNode is NULL");
    return '\0';
  }
  return node->item;
}

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
void* maps_getMapNodeType(mapNode_t* node)
{
  if (node == NULL) {
    log_v("map_getMapNodeType: mapNode is NULL");
    return NULL;
  }
  return node->type;
}

/** maps_setMapNodeItem
 * 
 * Sets the char item for a given mapNode
 * 
 * Caller provides:
 *  valid mapNode pointer
 *  char in the mapNode
*/
void maps_setMapNodeItem(mapNode_t* node, char item)
{
  if (node == NULL) {
    log_v("map_setMapNodeItem: mapNode is NULL");
    return;
  }
  node->item = item;
}

/** maps_getMapNodeType
 * 
 * Returns the void* type for a given mapNode
 * 
 * Caller provides:
 *  valid mapNode pointer
 *  valid void* type in the mapNode (player or gold)
*/
void maps_setMapNodeType(mapNode_t* node, void* type)
{
  if (node == NULL) {
    log_v("map_setMapNodeType: mapNode is NULL");
    return;
  }
  node->type = type;
}

/** maps_setTotalGoldLeft
 * 
 * Sets the integer total gold left in a map
*/
void maps_setTotalGoldLeft(map_t* map, int totalGoldLeft)
{
  if (map == NULL) {
    log_v("maps_setTotalGoldLeft: map is NULL");
    return;
  } else if (totalGoldLeft < 0) {
    log_d("maps_setTotalGoldLeft: totalGoldLeft value of %d is invalid (can't be negative)", totalGoldLeft);
    return;
  }
  map->totalGoldLeft = totalGoldLeft;
}

/** maps_getTotalGoldLeft
 * 
 * Gets the integer total gold left in a map
*/
int maps_getTotalGoldLeft(map_t* map)
{
  if (map == NULL) {
    log_v("maps_setTotalGoldLeft: map is NULL");
    return -1;
  }
  return map->totalGoldLeft;
}

/** maps_delete
 * 
 * Frees a given map from memory
 * 
 * Caller provides
 *  a valid map pointer
 * We guarantee:
 *  the map struct and all underlying structs are freed from memory
*/
void maps_delete(map_t* map)
{
  if (map == NULL) {
    log_v("maps_setTotalGoldLeft: map is NULL");
    return;
  }
  mapNode_t* node;
  if (map->grid!=NULL) {
    for (int r = 0; r < map->numRows; r++) {
      for (int c = 0; c < map->numCols; c++) {
        if ((node=map->grid[r][c]) != NULL) {
          free(node);
        }
      }
      if (map->grid[r] != NULL) {
        free(map->grid[r]);
      }
    }
    free(map->grid);
  }
  free(map);
}

/** mapNodeNew
 * 
 * Creates a new mapNode structure and returns its pointer
*/
mapNode_t* mapNodeNew(char item)
{
  // fine if item is NULsL, no need to check
  mapNode_t* node = mem_malloc_assert(sizeof(mapNode_t), "Unable to allocate memory for mapnode struct\n");
  node->item = item;
  node->isTransparent = (item == '.');
  node->isHallway = (item == '#');
  return node;
}

bool maps_isHallwayNode(mapNode_t* node) {
  if (node == NULL) {
    return false;
  }
  return node->isHallway;
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

static bool firstAndEighthOctant(map_t* map, int playerRow, int playerCol, int testRow, int testCol)
{
  int changeInRows = testRow - playerRow; // find change in rows and cols again
  int changeInCols = testCol - playerCol;

  double slope = (double)changeInRows/(double)changeInCols; // get the slope of the line
  double intersect; // declare some variables we'll use for tracking intersect points
  int intersectFloor;
  int intersectCeiling;

  for (int col = playerCol+1; col < testCol; col++) { // for each gridpoint along the col ( player position + 1 incremented by 1)
    intersect = (double)playerRow + (((double)col-(double)playerCol)*slope); // get the intersection on the row (doubel value)
    if (intersect == trunc(intersect)) { // intersects an integer gridpoint
      if (!map->grid[(int)trunc(intersect)][col]->isTransparent){ // if point isn't transparent
        return false;
      }
    } else {
      intersectFloor = (int)floor(intersect); // get the floor of the double intersect point
      intersectCeiling = (int)ceil(intersect); // get the ceiling of the double intersect point
      if (!map->grid[intersectFloor][col]->isTransparent && !map->grid[intersectCeiling][col]->isTransparent) { // if both points aren't transparent
        return false;
      }
    }
  }
  return true;
}

static bool secondAndThirdOctant(map_t* map, int playerRow, int playerCol, int testRow, int testCol)
{
  int changeInRows = testRow - playerRow; // find change in rows and cols again
  int changeInCols = testCol - playerCol;

  double slope = (double)changeInCols/(double)changeInRows; // get the slope of the line
  double intersect; // declare some variables we'll use for tracking intersect points
  int intersectFloor;
  int intersectCeiling;

  for (int row = playerRow-1; row > testRow; row--) { // for each gridpoint along the col ( player position + 1 incremented by 1)
    intersect = (double)playerCol + (((double)row-(double)playerRow)*slope); // get the intersection on the row (doubel value)
    if (intersect == trunc(intersect)) { // intersects an integer gridpoint
      if (!map->grid[row][(int)trunc(intersect)]->isTransparent){ // if point isn't transparent
        return false;
      }
    } else {
      intersectFloor = (int)floor(intersect); // get the floor of the double intersect point
      intersectCeiling = (int)ceil(intersect); // get the ceiling of the double intersect point
      if (!map->grid[row][intersectFloor]->isTransparent && !map->grid[row][intersectCeiling]->isTransparent) { // if both points aren't transparent
        return false;
      }
    }
  }
  return true;
}

static bool fourthAndFifthOctant(map_t* map, int playerRow, int playerCol, int testRow, int testCol)
{
  int changeInRows = testRow - playerRow; // find change in rows and cols again
  int changeInCols = testCol - playerCol;

  double slope = (double)changeInRows/(double)changeInCols; // get the slope of the line
  double intersect; // declare some variables we'll use for tracking intersect points
  int intersectFloor;
  int intersectCeiling;

  for (int col = playerCol-1; col > testCol; col--) { // for each gridpoint along the col ( player position + 1 incremented by 1)
    intersect = (double)playerRow + (((double)col-(double)playerCol)*slope); // get the intersection on the row (doubel value)
    if (intersect == trunc(intersect)) { // intersects an integer gridpoint
      if (!map->grid[(int)trunc(intersect)][col]->isTransparent){ // if point isn't transparent
        return false;
      }
    } else {
      intersectFloor = (int)floor(intersect); // get the floor of the double intersect point
      intersectCeiling = (int)ceil(intersect); // get the ceiling of the double intersect point
      if (!map->grid[intersectFloor][col]->isTransparent && !map->grid[intersectCeiling][col]->isTransparent) { // if both points aren't transparent
        return false;
      }
    }
  }
  return true;
}

static bool sixthAndSeventhOctant(map_t* map, int playerRow, int playerCol, int testRow, int testCol)
{
  int changeInRows = testRow - playerRow; // find change in rows and cols again
  int changeInCols = testCol - playerCol;

  double slope = (double)changeInCols/(double)changeInRows; // get the slope of the line
  double intersect; // declare some variables we'll use for tracking intersect points
  int intersectFloor;
  int intersectCeiling;

  for (int row = playerRow+1; row < testRow; row++) { // for each gridpoint along the col ( player position + 1 incremented by 1)
    intersect = (double)playerCol + (((double)row-(double)playerRow)*slope); // get the intersection on the row (doubel value)
    if (intersect == trunc(intersect)) { // intersects an integer gridpoint
      if (!map->grid[row][(int)trunc(intersect)]->isTransparent){ // if point isn't transparent
        return false;
      }
    } else {
      intersectFloor = (int)floor(intersect); // get the floor of the double intersect point
      intersectCeiling = (int)ceil(intersect); // get the ceiling of the double intersect point
      if (!map->grid[row][intersectFloor]->isTransparent && !map->grid[row][intersectCeiling]->isTransparent) { // if both points aren't transparent
        return false;
      }
    }
  }
  return true;
}
