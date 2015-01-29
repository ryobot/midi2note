// midi2note.cpp
//#include <typeinfo>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <string.h>
using namespace std;

#include "note_map.h"

var_data varData[2];
vector<key_value> items;
char buf[256];

int main(int argc, char *argv[])
{
    if ( argc < 1 ) {
        printf("usage : note2map [txt note file]\n", argv[1]);
        exit(1);
    }
    FILE *f;
    if ( NULL == ( f = fopen(argv[1], "r"))) {
        printf("file %s not found\n", argv[1]);
        exit(1);
    }
    int current = 0;
    int prev = 1;
    strcpy(buf, "000000 |           |           |           |           |           |           |");
    make_vars(buf, varData[prev]);
    while( fgets(buf, 255, f) != NULL ) {
        // variations:
        make_vars(buf, varData[current]);
        make_maps(varData[prev], varData[current], items);
        // output:
        for ( int i = 0; i < items.size(); i++ ) {
            printf("%.2f,%s\n", items[i].val, items[i].key);
        }
        // toggle buffer:
        int tmp = current;
        current = prev;
        prev = tmp;
    }
    fclose(f);
}