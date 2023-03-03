/** mapstest.c
 * 
 * A testing program for the maps module
 * 
 * Wall Street Djournal - CS50 Nuggets 23W
*/

#include <stdlib.h>
#include <stdio.h>
#include "maps.h"

int main()
{
  map_t* map = maps_new("../maps/small.txt");
  printf("%s",maps_basegrid(map));
}