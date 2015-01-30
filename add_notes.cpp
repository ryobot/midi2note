// add_notes.cpp
//#include <typeinfo>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <string.h>
using namespace std;

#include "note_map.h"

vector<key_value> ref_map;
vector<key_value> cur_map;
vector<notes> notes;

int main(int argc, char *argv[])
{
    if ( argc < 1 ) {
        printf("usage : add_notes [note file] [reference reduced map file] [frames to add(default:16)]\n", argv[1]);
        exit(1);
    }
    bool reduced = true;
    
    // load src notes:
    int src_len = load_note_file(argv[1], notes);
    
    // note2map
    // map2reduce
    
    // load reference:
    load_key_value_file(argv[2], ref_map, reduced);    
        
    // new frames:
    for (int i = 0; i < atoi(argv[3]); i++ ) {
        char new_note[256];
        make_new_frame(last_note, new_note, cur_map, ref_map);
    }
}