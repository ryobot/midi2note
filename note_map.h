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

#define MIN_NOTE_POS 9
#define MAX_NOTE_POS 82
#define NOTE_NUM_OFFSET 24

enum {
    VALUE_BY_NOTE_NUM,
    VALUE_DIVIDED_NOTE_NUM,
    VALUE_SAME_NOTE_NUM
};

struct var {
    char str[256];
    int note_cnt;
    
    int putstr(char* buf);
};

struct var_data {
    char original[256];
    vector<var> strs;
    vector<int> note_pos;
    int typebits;
    
    int note_cnt() {
        return note_pos.size();
    }
};

int get_vars(char* buf, var_data& vars );
int make_vars(char* buf, var_data& vars);
int make_vars_accept_none(char* buf, var_data& vars);
bool is_note_ch(char ch);
bool is_possible_map(char* buf_prev, char* buf_cur, char* org_prev, char* org_cur);
int make_map(char* buf_prev, char* buf_cur, float val, vector<key_value> &items);
int make_maps(var_data &prev, var_data &cur, vector<key_value> &items, int vMode);
int note2map(vector<notes> &notes, vector<key_value> &map, int vMode, int timing_res=4, bool verbose=false);
int note2mask(vector<notes> &notes, char* mask);
int note2vmap(vector<notes> &notes, vector<key_value> &vmap);

void init_note_num();
char* get_n_note(int num);
char* get_c_note(int num);

#endif