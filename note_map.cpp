// note_map.cpp

#include "note_map.h"

int var::putstr(char* buf) {
    strcpy(str, buf);
    int cnt = 0;
    for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
        if ( str[i] == 'o' || str[i] == '+' ) {
            cnt++;
        }
    }
    note_cnt = cnt;
    return cnt;
}

char c_note_nums[128][8];
char n_note_nums[128][8];
char dim1[64][8] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
                                 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
                                 'W', 'X', 'Y', 'Z', 'a', 'b', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
                                 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '!', '#', '$', '%' };

void init_note_num() {
    for ( int i = 0; i < 128; i++ ) {
        sprintf(c_note_nums[i], "c%03d", i);
        sprintf(n_note_nums[i], "n%03d", i);
    }
}

char* get_n_note(int num) {
    return n_note_nums[num];
}

char* get_c_note(int num) {
    return c_note_nums[num];
}

int get_vars(char* buf, var_data& vars ) {
    vars.note_pos.clear();
    vars.typebits = 0;
    for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
        if ( buf[i] == 'o' || buf[i] == '+' ) {
            vars.note_pos.push_back(i);
            if ( buf[i] == '+' ) vars.typebits += 1;
            vars.typebits <<= 1;
        }
    }
    return vars.note_pos.size();
}

int make_vars(char* buf, var_data& vars) {
    vars.strs.clear();
    int num_pos = get_vars(buf, vars);
    // original:
    var newstr;
    newstr.putstr(buf);
    int num_notes = newstr.note_cnt;
    vars.strs.push_back(newstr);
    // missings:
    int patterns = 1;
    for (int i = 0; i < num_pos; i++) {
        patterns *= 2;
    }
    // no notes (i = patterns - 1) is not a variation:
    for (int i = 1; i < patterns - 1; i++) {
        strcpy(newstr.str, buf);
        newstr.note_cnt = num_notes;
        for (int pos = 0; pos < num_pos; pos++) {
            if ( (i >> pos) % 2 ) {
                newstr.str[vars.note_pos[pos]] = ' ';
                newstr.note_cnt--;
            }
        }
        vars.strs.push_back(newstr);
    }
    return vars.strs.size();
}

int make_vars_accept_none(char* buf, var_data& vars) {
    vars.strs.clear();
    int num_pos = get_vars(buf, vars);
    // original:
    var newstr;
    newstr.putstr(buf);
    int num_notes = newstr.note_cnt;
    vars.strs.push_back(newstr);
    // missings:
    int patterns = 1;
    for (int i = 0; i < num_pos; i++) {
        patterns *= 2;
    }
    // no notes (i = patterns - 1) is a variation:
    for (int i = 1; i < patterns ; i++) {
        strcpy(newstr.str, buf);
        newstr.note_cnt = num_notes;
        for (int pos = 0; pos < num_pos; pos++) {
            if ( (i >> pos) % 2 ) {
                newstr.str[vars.note_pos[pos]] = ' ';
                newstr.note_cnt--;
            }
        }
        vars.strs.push_back(newstr);
    }
    return vars.strs.size();
}

bool is_note_ch(char ch) {
    if ( ch == ' ' || ch == '-' || ch == '|' ) {
        return false;
    }
    return true;
}

bool is_possible_map(char* buf_prev, char* buf_cur, char* org_prev, char* org_cur) {
    for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
        // cannot continue note without previous note:
        if ( !is_note_ch(buf_prev[i]) && buf_cur[i] == '+') {
            return false;
        }
        // variations continue -> off is not valid:
        if ( is_note_ch(buf_prev[i]) && org_cur[i] == '+' && buf_cur[i] != '+' ) {
            return false;
        }
    }
    return true;
}

bool is_possible_map_indexed(char* buf_prev, char* buf_cur, char* org_prev, char* org_cur, vector<int> &checkIndex) {
    for (int i = 0; i < checkIndex.size(); i++ ) {
        // cannot continue note without previous note:
        if ( !is_note_ch(buf_prev[checkIndex[i]]) && buf_cur[checkIndex[i]] == '+') {
            return false;
        }
        // variations continue -> off is not valid:
        if ( is_note_ch(buf_prev[checkIndex[i]]) && org_cur[checkIndex[i]] == '+' && buf_cur[checkIndex[i]] != '+' ) {
            return false;
        }
    }
    return true;
}

int get_possible_map_index(var_data &prev, var_data &cur, vector<int> &checkIndex) {
    checkIndex.clear();
    for (int ci = 0; ci < cur.note_pos.size(); ci++ ) {
        for (int pi = 0; pi < prev.note_pos.size(); pi++ ) {
            if ( prev.note_pos[pi] == cur.note_pos[ci] && cur.strs[0].str[cur.note_pos[ci]] == '+' ) {
            //if ( prev.note_pos[pi] == cur.note_pos[ci] ) {
                checkIndex.push_back(prev.note_pos[pi]);
            }
        }
    }
    return checkIndex.size();
}

int make_map(char* buf_prev, char* buf_cur, float val, vector<key_value> &items) {
    key_value item;
    int num_tones = 0;
    char wbuf[16];
    strcpy(item.key, "");
    int offset = NOTE_NUM_OFFSET - MIN_NOTE_POS;
    
    // previous notes:
    for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
        if ( buf_prev[i] == 'o' ) {
            strcat(item.key, n_note_nums[i + offset]);
            num_tones++;
        }
        else if ( buf_prev[i] == '+' ) {
            strcat(item.key, c_note_nums[i + offset]);
            num_tones++;
        }
    }
    strcat(item.key, ">");
    
    // current notes:
    for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
        if ( buf_cur[i] == 'o' ) {
            strcat(item.key, n_note_nums[i + offset]);
            num_tones++;
        }
        else if ( buf_cur[i] == '+' ) {
            strcat(item.key, c_note_nums[i + offset]);
            num_tones++;
        }
    }
    item.val = (float)num_tones;
    item.init_val = 0;
    item.x_val = -1;
    item.updated = true;
    items.push_back(item);
}

int make_maps(var_data &prev, var_data &cur, vector<key_value> &items) {
    items.clear();
    float total = (float)(prev.note_cnt() + cur.note_cnt());
    int possible_map_cnt = 0;
    vector<int> checkIndex;
    int checks = get_possible_map_index(prev, cur, checkIndex);
    for (int i = 0; i < prev.strs.size(); i++) {
        for (int j = 0; j < cur.strs.size(); j++) {
            if ( i == 0 && j == 0 ) {
                make_map(prev.strs[0].str, cur.strs[0].str, 1.0, items);
                //possible_map_cnt++;
            }
            else {
                //if ( cur.typebits == 0 || is_possible_map(prev.strs[i].str, cur.strs[j].str, prev.strs[0].str, cur.strs[0].str) ) {
                if ( cur.typebits == 0 || is_possible_map_indexed(prev.strs[i].str, cur.strs[j].str, prev.strs[0].str, cur.strs[0].str, checkIndex) ) {
                    //float val = (float)(prev.strs[i].note_cnt + cur.strs[j].note_cnt)/(float)total;
                    float val = 1.0;
                    make_map(prev.strs[i].str, cur.strs[j].str, val, items);
                    possible_map_cnt++;
                }
            }
        }
    }
    /*
    for (int i = 1; i < items.size(); i++) {
        items[i].val /= (float)possible_map_cnt;
    }
     */
}

int note2map(vector<notes> &notes, vector<key_value> &map, int timing_res, bool verbose) {
    vector<key_value> items;
    var_data varData[2];
    int current = 0;
    int prev = 1;
    char buf[256];
    
    map.clear();
    strcpy(buf, "000000 : |           |           |           |           |           |           |");
    make_vars(buf, varData[prev]);
    for (int j = 0; j < notes.size(); j++ ) {
        //printf ("%d\n", notes[j].time);
        // variations:
        make_vars(notes[j].note, varData[current]);
        make_maps(varData[prev], varData[current], items);
        // output:
        char timing_str[16];
        if ( timing_res ) {
            int timing = (notes[j].time / 240) % timing_res;
            sprintf(timing_str, "t%d%d", timing_res, timing);
        }
        else {
            strcpy(timing_str, "t00");
        }
        for ( int i = 0; i < items.size(); i++ ) {
            strcat(items[i].key, timing_str);
            map.push_back(items[i]);
        }
        // toggle buffer:
        int tmp = current;
        current = prev;
        prev = tmp;
        if ( verbose && (j % 10) == 0 ) {
            printf ("notes %d - maps %d\n", j, map.size());
        }
    }
    return(map.size());
}

int note2mask(vector<notes> &notes, char* mask) {
    strcpy(mask, "000000 : |           |           |           |           |           |           |");
    for (int j = 0; j < notes.size(); j++ ) {
        for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
            if ( notes[j].note[i] == 'o' ) {
                mask[i] = 'o';
            }
        }
    }
    int cnt = 0;
    for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
        if ( mask[i] == 'o' ) {
            cnt++;
        }
    }
    return(cnt);
}

int note2vmap(vector<notes> &notes, vector<key_value> &vmap) {
    int voices[73] = {0};
    char v[4];
    strcpy(v, "0c+");
    char v_str[16];
    strcpy(v_str, "cvdwefxgyazb");
    char oct_str[8];
    strcpy(oct_str, "0123456");
    key_value item;
    vmap.clear();
    for (int j = 0; j < notes.size(); j++ ) {
        // output:
        for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
            if ( notes[j].note[i] == 'o' ) {
                voices[(i - MIN_NOTE_POS)] = 1;
            }
            if ( notes[j].note[i] == '+' ) {
                voices[(i - MIN_NOTE_POS)] = -1;
            }
        }
        strcpy(item.key, "@");
        item.val = 1.0;
        item.init_val = 0;
        item.x_val = -1;
        item.updated = true;
        for ( int i = 0; i < 73; i++ ) {
            if ( voices[i] ) {
                v[0] = oct_str[i / 12];
                v[1] = v_str[i % 12];
                if ( voices[i] > 0 ) {
                    v[2] = '+';
                } else {
                    v[2] = '-';
                }
                strcat(item.key, v);
                voices[i] = 0;
            }
        }
        vmap.push_back(item);
    }
    return(vmap.size());
}

