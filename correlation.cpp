// unittest.cpp
//#include <typeinfo>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <string.h>
using namespace std;

#include "key_value.h"

vector<key_value> items1;
vector<key_value> items2;

int main(int argc, char *argv[])
{
    if ( argc < 2 ) {
        printf("usage : correlation [txt map file1] [txt map file2]\n");
        exit(1);
    }
    
    int argc_pos = 1;
    bool reduced = true;
    
    // load map:
    load_key_value_file(argv[argc_pos++], items1, reduced);
    printf ("item1 : %d data\n", items1.size());
    load_key_value_file(argv[argc_pos++], items2, reduced);
    printf ("item2 : %d data\n", items2.size());

    float corr = correlation(items1, items2);
    printf ("correlation : %.8f\n", corr);
    exit(0);
}