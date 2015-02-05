// key_value.h
#ifndef KEY_VALUE_H
#define KEY_VALUE_H

#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <math.h>
using namespace std;

struct key_value {
    float val;
    char key[128];

    void init(char* buf);
};

int load_key_value_file(char* filename, vector<key_value> &items, bool reduced=false);
int map2reduce(vector<key_value> &items);
float value_for_key(char* key, vector<key_value> &items);
float standard(vector<key_value> &items);
float x_standard(vector<key_value> &items_a, vector<key_value> &items_b);
float correlation(vector<key_value> &items_a, vector<key_value> &items_b);
int add_map(vector<key_value> &items, vector<key_value> &add);
int mapcopy(vector<key_value> &dst, vector<key_value> &src);

#endif
