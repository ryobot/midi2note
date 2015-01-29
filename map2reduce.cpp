// midi2note.cpp
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
    if ( argc < 1 ) {
        printf("usage : map2reduce [txt map file]\n", argv[1]);
        exit(1);
    }
    load_key_value_file(argv[1], items);
    for ( int i = 0; i < items.size(); i++ ) {
        printf("%.2f,%s\n", items[i].val, items[i].key);
    }
    exit(0);
}