// unittest.cpp
//#include <typeinfo>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <string.h>
using namespace std;

#include "note_map.h"

vector<key_value> items1;
vector<key_value> items2;
vector<notes> notes;

int main(int argc, char *argv[])
{
    // initializations:
    setvbuf(stdout, (char *)NULL, _IONBF, 0);
    init_note_num();
    
    if ( argc < 2 ) {
        printf("usage : correlation [txt note file1] [txt note file2] [-v:set voice mode]\n");
        exit(1);
    }
    
    bool voice_mode = false;
    if (  argc > 3 && strcmp(argv[3],"-v") == 0 ) {
        voice_mode = true;
        printf ("voice mode\n");
    }
    
    notes.clear();
    load_note_file(argv[1], notes);
    printf ("notes 1 : %d frames\n", notes.size());
    if ( voice_mode ) note2vmap(notes, items1);
    else note2map(notes, items1, 4, false);
    printf ("map 1 : %d data\n", items1.size());
    map2reduce(items1);
    printf ("map 1 reduced : %d data\n", items1.size());

    notes.clear();
    load_note_file(argv[2], notes);
    printf ("notes 2 : %d frames\n", notes.size());
    if ( voice_mode ) note2vmap(notes, items2);
    else note2map(notes, items2, 4, false);
    printf ("map 2 : %d data\n", items2.size());
    map2reduce(items2);
    printf ("map 2 reduced : %d data\n", items2.size());

    //correlator c;
    
    //float corr = c.init(items1, items2);
    float corr = correlation(items1, items2);
    printf ("correlation : %.8f\n", corr);
    exit(0);
}