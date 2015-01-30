// note_map.h
#ifndef NOTE_MAP_H
#define NOTE_MAP_H

#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <string.h>
using namespace std;

#include "key_value.h"
#include "notes.h"

#define MIN_NOTE_POS 7
#define MAX_NOTE_POS 80
#define NOTE_NUM_OFFSET 24

struct var {
    char str[256];
    int note_cnt;
    
    int putstr(char* buf);
};

struct var_data {
    char original[256];
    vector<var> strs;
    vector<int> note_pos;
    
    int note_cnt() {
        return note_pos.size();
    }
};

int get_vars(char* buf, var_data& vars );
int make_vars(char* buf, var_data& vars);
bool is_note_ch(char ch);
bool is_possible_map(char* buf_prev, char* buf_cur, char* org_prev, char* org_cur);
int make_map(char* buf_prev, char* buf_cur, float val, vector<key_value> &items);
int make_maps(var_data &prev, var_data &cur, vector<key_value> &items);
int note2map(vector<notes> &notes, vector<key_value> &map);

#endif