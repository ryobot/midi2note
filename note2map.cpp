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
    note2map(notes, map);
    for (int i = 0; i < map.size(); i++ ) {
        printf("%.2f,%s\n", map[i].val, map[i].key);
    }
}