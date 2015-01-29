// midi2note.cpp
//#include <typeinfo>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <string.h>
using namespace std;

#define MIN_NOTE_POS 7
#define MAX_NOTE_POS 80
#define NOTE_NUM_OFFSET 24

struct var {
    char str[256];
    int note_cnt;
    
    int putstr(char* buf) {
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
};

struct var_data {
    char original[256];
    vector<var> strs;
    vector<int> note_pos;
    
    int note_cnt() {
        return note_pos.size();
    }
};

var_data varData[2];

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

int make_map(char* buf_prev, char* buf_cur, float val) {
    char map[256];
    char wbuf[16];
    sprintf(map, "%.2f,", val);
    int offset = NOTE_NUM_OFFSET - MIN_NOTE_POS;
    // previous notes:
    for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
        if ( buf_prev[i] == 'o' ) {
            sprintf(wbuf, "n%03d", i + offset);
            strcat(map, wbuf);
        }
        if ( buf_prev[i] == '+' ) {
            sprintf(wbuf, "c%03d", i + offset);
            strcat(map, wbuf);
        }
    }
    strcat(map, ">");
    // current notes:
    for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
        if ( buf_cur[i] == 'o' ) {
            sprintf(wbuf, "n%03d", i + offset);
            strcat(map, wbuf);
        }
        if ( buf_cur[i] == '+' ) {
            sprintf(wbuf, "c%03d", i + offset);
            strcat(map, wbuf);
        }
    }
    printf("%s\n", map);
}

int main(int argc, char *argv[])
{
    if ( argc < 1 ) {
        printf("usage : note2map [txt note file]\n", argv[1]);
        exit(1);
    }
    FILE *f;
    if ( NULL == ( f = fopen(argv[1], "r"))) {
        printf("file %s not found\n", argv[1]);
        exit(1);
    }
    char buf[2][256];
    int current = 0;
    int prev = 1;
    strcpy(buf[prev], "000000 |           |           |           |           |           |           |");
    make_vars(buf[prev], varData[prev]);
    while( fgets(buf[current], 255, f) != NULL ) {
        // variations:
        make_vars(buf[current], varData[current]);
        float total = (float)(varData[prev].note_cnt() + varData[current].note_cnt());
        for (int i = 0; i < varData[prev].strs.size(); i++) {
            for (int j = 0; j < varData[current].strs.size(); j++) {
                if ( i == 0 && j == 0 ) {
                    make_map(buf[prev], buf[current], 1.0);
                }
                else {
                    if ( is_possible_map(varData[prev].strs[i].str, varData[current].strs[j].str, buf[prev], buf[current]) ) {
                        float val = (float)(varData[prev].strs[i].note_cnt + varData[current].strs[j].note_cnt)/(float)total;
                        make_map(varData[prev].strs[i].str, varData[current].strs[j].str, val);
                    }
                }
            }
        }
        // toggle buffer:
        int tmp = current;
        current = prev;
        prev = tmp;
    }
    fclose(f);
}