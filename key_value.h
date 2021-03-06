// key_value.h
#ifndef KEY_VALUE_H
#define KEY_VALUE_H

#include <cstdlib>
#include <vector>
#include <string>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <algorithm>
#include <functional>
using namespace std;

#define TABLE_LENGTH 1023

struct key_value {
    float val;
    float init_val;
    float x_val;
    char key[128];
    bool updated;

    void init(char* buf);
};

struct key_contents {
    vector<int> prev_on;
    vector<int> prev_continue;
    vector<int> cur_on;
    vector<int> cur_continue;
    char prevKey[64];
    char nextKey[64];
    char timing_str[8];
    
    void init(char* key);
    void dump();
};

struct key_value_hash {
    vector<key_value> table[TABLE_LENGTH];
    
    void init(vector<key_value> &items);
    float value_for_key_in_hash(char* key);
    void dump_table(char* key);
};

struct correlator {
    float correlation;
    float standard_a;
    float standard_b;
    float x_standard;
    
    float calc_if_a(vector<key_value> &items_a, vector<key_value> &items_b);
    float calc_if_a_kvh(vector<key_value> &items_a, key_value_hash &kvh);
    float init(vector<key_value> &items_a, vector<key_value> &items_b);
};

int load_key_value_file(char* filename, vector<key_value> &items, bool reduced=false);
int map2reduce(vector<key_value> &items);
float value_for_key(char* key, vector<key_value> &items);
int index_for_key(char* key, vector<key_value> &items);
float value_for_key_lg(char* key, vector<key_value> &items);
float value_for_key_lg_debug(char* key, vector<key_value> &items);
int index_for_key_lg(char* key, vector<key_value> &items);
float standard(vector<key_value> &items);
float x_standard(vector<key_value> &items_a, vector<key_value> &items_b);
float correlation(vector<key_value> &items_a, vector<key_value> &items_b);
int add_map(vector<key_value> &items, vector<key_value> &add);
int add_map_insert(vector<key_value> &items, vector<key_value> &add);
int add_map_old(vector<key_value> &items, vector<key_value> &add);
int mapcopy(vector<key_value> &dst, vector<key_value> &src);
void resumeKey(key_value &item);
void map_sort(vector<key_value> &items);
size_t hash3bit(char *key);

#endif
