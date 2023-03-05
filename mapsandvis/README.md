# Maps & Visibility Module - CS50 Nuggets
The maps and visibility module for the cs50 nuggets project

Wall Street Djournal - CS50 Nuggets 23W

## Compilation instructions
Clean the directory with `make clean` and compile the maps.a archive file and the mapstest file with `make all`.

## Implementation notes
We follow our implementation spec closely while writing the maps module, please refer to that as it is outlined in the main directory.
There are no known deviations from spec.

## Maps test
Run the maps test with `make test` (in the maps directory) and see that we can load a file and print it out, along with the map as visible from a certain point. This is also run with valgrind so you can see no memory leaks when loading and deleting the map.

## Custom map
We created our own custom map (as required in the assignment details) and stored it under `maps/wall-street-djournal.txt`. 
