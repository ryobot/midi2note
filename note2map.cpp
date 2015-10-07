// midi2note.cpp
//#include <typeinfo>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <string.h>
using namespace std;

#include "note_map.h"

vector<key_value> map;
vector<notes> notes;

int main(int argc, char *argv[])
{
    // initializations:
    setvbuf(stdout, (char *)NULL, _IONBF, 0);
    init_note_num();
    
    if ( argc < 1 ) {
        printf("usage : note2map [txt note file]\n");
        exit(1);
    }
    bool verbose = false;
    if ( argc == 3 && strcmp(argv[2], "-v") == 0 ) {
        verbose = true;
    }
    load_note_file(argv[1], notes);
    if (verbose)    printf ("notes length:%d\n", notes.size());
    if (verbose)    printf ("making map...");
    note2map(notes, map, 4, verbose);
    //note2vmap(notes, map);
    if (verbose)    printf ("%d items\n", map.size());
    if (verbose)    printf ("reducing map...");
    map2reduce(map);
    //map_sort(map);
    if (verbose)    printf ("%d items\n", map.size());
    if (verbose)    {
        for (int i = 0; i < 20; i++ ) {
            printf("%.5f,%s\n", map[i].val, map[i].key);
        }
    } else {
        for (int i = 0; i < map.size(); i++ ) {
            printf("%.5f,%s\n", map[i].val, map[i].key);
        }
    }
}