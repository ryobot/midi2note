// unittest.cpp
//#include <typeinfo>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <string.h>
using namespace std;

#include "key_value.h"

vector<key_value> items;

int main(int argc, char *argv[])
{
    if ( argc < 2 ) {
        printf("usage : unittest [txt map file]\n");
        exit(1);
    }
    
    int argc_pos = 1;
    bool reduced = true;
    
    // load map:
    load_key_value_file(argv[argc_pos++], items, reduced);
    printf ("item : %d data\n", items.size());
    
    char key[128];
    
    printf("key?:");
    scanf("%s", key);
    printf ("index for key : %d\n", index_for_key(key, items));
    printf ("index for key (lg) : %d\n", index_for_key_lg(key, items));
    printf ("value for key : %.8f\n", value_for_key(key, items));
    printf ("value for key (lg) : %.8f\n", value_for_key_lg(key, items));
    
    /*
    for ( int i = 0; i < items.size(); i++ ) {
        strcpy(key, items[i].key);
        if ( index_for_key(key, items) != index_for_key_lg(key, items) ) {
            printf ("index for key : %d\n", index_for_key(key, items));
            printf ("index for key (lg) : %d\n", index_for_key_lg(key, items));
        }
        if ( value_for_key(key, items) != value_for_key_lg(key, items) ) {
            printf ("value for key : %.8f\n", value_for_key(key, items));
            printf ("value for key (lg) : %.8f\n", value_for_key_lg(key, items));
        }
        strcat(key, "o");
        if ( index_for_key(key, items) != index_for_key_lg(key, items) ) {
            printf ("index for key : %d\n", index_for_key(key, items));
            printf ("index for key (lg) : %d\n", index_for_key_lg(key, items));
        }
        if ( value_for_key(key, items) != value_for_key_lg(key, items) ) {
            printf ("value for key : %.8f\n", value_for_key(key, items));
            printf ("value for key (lg) : %.8f\n", value_for_key_lg(key, items));
        }
    }*/
    exit(0);
}