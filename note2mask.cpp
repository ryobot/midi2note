// midi2note.cpp
//#include <typeinfo>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <string.h>
using namespace std;

#include "note_map.h"

vector<notes> notes;

int main(int argc, char *argv[])
{
    if ( argc < 1 ) {
        printf("usage : note2mask [txt note file]\n");
        exit(1);
    }
    load_note_file(argv[1], notes);
    char mask[256];
    note2mask(notes, mask);
    printf("%s\n", mask);
}