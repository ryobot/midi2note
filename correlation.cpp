// correlation.cpp
//#include <typeinfo>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <math.h>
using namespace std;

#include "key_value.h"

vector<key_value> x_items;
vector<key_value> y_items;

float standard(vector<key_value> &items) {
    float st = 0;
    for ( int i = 0; i < items.size(); i++ ) {
        st += items[i].val*items[i].val;
    }
    return sqrtf(st);
}

float x_standard(vector<key_value> &items_a, vector<key_value> &items_b) {
    float xst = 0;
    for ( int i = 0; i < items_a.size(); i++ ) {
        xst += items_a[i].val*value_for_key(items_a[i].key, items_b);
    }
    return xst;
}

int main(int argc, char *argv[])
{
    if ( argc < 2 ) {
        printf("usage : correlation [txt map file 1] [txt map file 2]\n");
        printf("usage : correlation -r [txt reduced file 1] [txt reduced file 2]\n");
        exit(1);
    }
    
    // option reduced or not:
    int argc_pos = 1;
    bool reduced = false;
    if ( strcmp( argv[argc_pos], "-r" ) == 0 ) {
        reduced = true;
        argc_pos++;
    }
    
    // file1:
    load_key_value_file(argv[argc_pos++], x_items, reduced);    
    // file2:
    load_key_value_file(argv[argc_pos++], y_items, reduced);
    
    float xcor = x_standard(x_items, y_items)/(standard(x_items)*standard(y_items));

    printf ("item x : %d data\n", x_items.size());
    printf ("item y : %d data\n", y_items.size());
    printf ("cross corr. : %.4f\n", xcor);
    exit(0);
}