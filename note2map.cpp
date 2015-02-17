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
    if ( argc < 1 ) {
        printf("usage : note2map [txt note file]\n");
        exit(1);
    }
    load_note_file(argv[1], notes);
    //printf ("making map...");
    note2map(notes, map);
    //printf ("%d items\n", map.size());
    //printf ("reducing map...");
    map2reduce(map);
    //map_sort(map);
    //printf ("%d items\n", map.size());
    //for (int i = 0; i < 20; i++ ) {
    for (int i = 0; i < map.size(); i++ ) {
        printf("%.5f,%s\n", map[i].val, map[i].key);
    }
}