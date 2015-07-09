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

int get_vars(char* buf, var_data& vars ) {
    vars.note_pos.clear();
    for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
        if ( buf[i] == 'o' || buf[i] == '+' ) {
            vars.note_pos.push_back(i);
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
    vars.strs.push_back(newstr);
    // missings:
    int patterns = 1;
    for (int i = 0; i < num_pos; i++) {
        patterns *= 2;
    }
    // no notes (i = patterns - 1) is not a variation:
    for (int i = 1; i < patterns - 1; i++) {
        newstr.putstr(buf);
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

int make_map(char* buf_prev, char* buf_cur, float val, vector<key_value> &items) {
    //printf("\e[32m%s\e[m\n", buf_prev);
    //printf("\e[34m%s\e[m\n\n", buf_cur);
    key_value item;
    char wbuf[16];
    strcpy(item.key, "");
    int offset = NOTE_NUM_OFFSET - MIN_NOTE_POS;
    // previous notes:
    for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
        if ( buf_prev[i] == 'o' ) {
            sprintf(wbuf, "n%03d", i + offset);
            strcat(item.key, wbuf);
        }
        if ( buf_prev[i] == '+' ) {
            sprintf(wbuf, "c%03d", i + offset);
            strcat(item.key, wbuf);
        }
    }
    strcat(item.key, ">");
    // current notes:
    for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
        if ( buf_cur[i] == 'o' ) {
            sprintf(wbuf, "n%03d", i + offset);
            strcat(item.key, wbuf);
        }
        if ( buf_cur[i] == '+' ) {
            sprintf(wbuf, "c%03d", i + offset);
            strcat(item.key, wbuf);
        }
    }
    item.val = val;
    items.push_back(item);
}

int make_maps(var_data &prev, var_data &cur, vector<key_value> &items) {
    items.clear();
    float total = (float)(prev.note_cnt() + cur.note_cnt());
    int possible_map_cnt = 0;
    for (int i = 0; i < prev.strs.size(); i++) {
        for (int j = 0; j < cur.strs.size(); j++) {
            if ( i == 0 && j == 0 ) {
                make_map(prev.strs[0].str, cur.strs[0].str, 1.0, items);
                //possible_map_cnt++;
            }
            else {
                if ( is_possible_map(prev.strs[i].str, cur.strs[j].str, prev.strs[0].str, cur.strs[0].str) ) {
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

int note2map(vector<notes> &notes, vector<key_value> &map, int timing_res) {
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
