/** mapstest.c
 * 
 * A testing program for the maps module
 * 
 * Loads the map we created
 * Prints out the base map
 * Prints out the map only as visible from a certain point
 * Frees the map
 * 
 * Wall Street Djournal - CS50 Nuggets 23W
*/

#include <stdlib.h>
#include <stdio.h>
#include "maps.h"

/** local function definition */
static char* getVisibleGrid(map_t* map, int row, int col);

/** main
 * 
 * Loads the map we created
 * Prints out the base map
 * Prints out the map only as visible from a certain point
 * Frees the map
 * 
 * Exit 0 - success
 * Exit 1 - unable to load map
*/
int main()
{
  map_t* map = maps_new("../maps/wall-street-djournal.txt"); // load the map we created
  if (map == NULL) {
    exit(1);
  }
  char* basegrid = maps_basegrid(map);
  printf("=====maps_basegrid=====\n%s\n",basegrid); // print out the base map

  char* visibleGrid = getVisibleGrid(map, 5, 5);
  printf("-------------------\n=====getVisibleGrid=====\n%s\n",visibleGrid); // print out map only visible from given point

  maps_delete(map); // free the map
  free(basegrid);
  free(visibleGrid);
  exit(0);
}

/** getVisibleGrid
 * 
 * Renders the map into the ascii string but only the points visible from a certain row and column
 * 
 *Caller provides:
 *  valid map pointer
 *  valid row integer (in bounds of map)
 *  valid col integer (in bounds of map)
 * We return: a string that's the ascii-rendered map with each row
 *  separated by new line, and null terminated for grid with only points visible from given row,col
 *  Null if invalid map
 * Caller is responsible for later freeing returned char* memory 
 * 
*/
static char* getVisibleGrid(map_t* map, int row, int col)
{
  if (map == NULL) {// validate args
    return NULL;
  } else if (row < 0 || row >= maps_getRows(map)) {
    return NULL;
  } else if (col < 0 || col >= maps_getCols(map)) {
    return NULL;
  }
  int numrows = maps_getRows(map);
  int numcols = maps_getCols(map);
	char* mapstring = mem_calloc_assert((numrows)*(numcols+1)+1, sizeof(char), "Unable to allocate memory for map string"); // allocate memory for string holding grid
	char* charptr = mapstring;
  for (int r = 0; r < numrows; r++) {// for each row r in the map (starting from 0)
    for (int c = 0; c < numcols; c++) {// 	for each column c in the map (starting from 0)
      mapNode_t* node = (maps_getMapNode(map, c, r));
      if (node != NULL) { // NULL check for node
        if (maps_isVisible(map, col, row, c, r)) { // if currently visible to given column and row (passed as x and y)
          if (col == c && row == r) { // if it's the poin
            *(charptr++) = '@'; // if it's the own player
          } else {
            *(charptr++) = maps_getMapNodeItem(node);
          }
        } else { // not currently visible to player
        *(charptr++) = ' '; 
        }
      } else { // null mapNode
        *(charptr++) = ' ';
      }
    }
    *(charptr++) = '\n';// 	add a new line to the string (for next row)
  }
	*(charptr++) = '\0'; // make last char a terminating null
	return mapstring; // return the string
}