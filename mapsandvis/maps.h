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

typedef struct map map_t;

typedef struct matrixIndex {
  int row;
  int col;
} matrixIndex_t;

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