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
  map_t* map = maps_new("../maps/main.txt");
  //printf("%s\n",maps_basegrid(map));
  printf("%s\n",maps_visiblebasegrid(map, 15, 34));
  printf("\n%d\n",isVisible(map,34,15,61,13));
  printf("true:%d, false:%d\n",true,false);
}