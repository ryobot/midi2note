// correlation.cpp
//#include <typeinfo>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <string.h>
using namespace std;

#include "key_value.h"

vector<key_value> x_items;
vector<key_value> y_items;

int main(int argc, char *argv[])
{
    if ( argc < 2 ) {
        printf("usage : correlation [txt map file 1] [txt map file 2]\n");
        printf("usage : correlation -n [txt reduced file 1] [txt reduced file 2]\n");
        exit(1);
    }
    
    // option reduced or not:
    int argc_pos = 1;
    bool reduced = true;
    if ( strcmp( argv[argc_pos], "-n" ) == 0 ) {
        reduced = false;
        argc_pos++;
    }
    
    // file1:
    load_key_value_file(argv[argc_pos++], x_items, reduced);    
    // file2:
    load_key_value_file(argv[argc_pos++], y_items, reduced);
    
    float xcor = correlation(x_items, y_items);

    printf ("item x : %d data\n", x_items.size());
    printf ("item y : %d data\n", y_items.size());
    printf ("cross corr. : %.4f\n", xcor);
    exit(0);
}