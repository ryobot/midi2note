// add_notes.cpp
//#include <typeinfo>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#include "note_map.h"

vector<key_value> ref_map;
vector<key_value> cur_map;
vector<notes> cur_notes;

#define MAX_NOTES_GENERATOR 4
#define INIT_NOTES_SEPARATION 12
#define MAX_MASKS 10

bool continue_if_minus = false;
int max_masks = MAX_MASKS;

clock_t add_map_time, correlation_time, generate_time, while_time;

struct note_generator {
    int num_notes;
    int index[MAX_NOTES_GENERATOR];
    char type[MAX_NOTES_GENERATOR];
    char buf[128];
    char ref[128];
    char mask[128];
    bool completed;
    int beat_pos;
    int generators_num;
    
    void init(notes &last_note, const char* _mask) {
        strcpy(mask, _mask);
        generators_num = MAX_NOTES_GENERATOR;
        for(int i = 0; i < MAX_NOTES_GENERATOR; i++) {
            index[i] = MIN_NOTE_POS + i*INIT_NOTES_SEPARATION;
            type[i] = 'o';
        }
        strcpy(ref, last_note.note);
        sprintf(buf, "%06d : |           |           |           |           |           |           |", last_note.time + 240);
        beat_pos = (last_note.time + 240) % 960;
        completed = false;
    }
    void iterate(int in) {
        if ( is_note_ch(ref[index[in]]) && type[in] == 'o' ) {
            type[in] = '+';
        } else {
            index[in]++;
            type[in] = 'o';
        }
        if ( index[in] >= MAX_NOTE_POS ) {
            if ( in > 0 ) {
                iterate(in - 1);
                index[in] = index[in - 1] + 1;
                if ( index[in] < MIN_NOTE_POS + in*INIT_NOTES_SEPARATION ) index[in] = MIN_NOTE_POS + in*INIT_NOTES_SEPARATION;
            } else {
                if ( --generators_num == 0 ) {
                    completed = true;
                } else {
                    for(int i = 0; i < generators_num; i++) {
                        index[i] = MIN_NOTE_POS + i*INIT_NOTES_SEPARATION;
                        type[i] = 'o';
                    }
                }
            }
        }
    }
    char* get_note() {
        for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
            if ( (i - MIN_NOTE_POS) % 12 == 0 ) {
                buf[i] = '|';
            } else {
                if ( beat_pos == 0 ) {
                    buf[i] = '-';
                } else {
                    buf[i] = ' ';
                }
            }
            for(int j = 0; j < generators_num; j++) {
                if ( index[j] == i ) {
                    buf[i] = type[j];
                    break;
                }
            }
        }
        iterate(generators_num - 1);
        return buf;
    }
    bool maskChk() {
        if ( mask[0] == 0 ) {
            return(true);
        }
        for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
            if ( buf[i] == 'o' && !(mask[i] == 'o' || mask[i] == 'x') ) {
                return(false);
            }
            if ( buf[i] == '+' && !(mask[i] == '+' || mask[i] == 'x') ) {
                return(false);
            }
        }
        return(true);
    }
    bool continueChk() {
        for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
            if ( buf[i] == '+' && !is_note_ch(ref[i]) ) {
                return(false);
            }
        }
        return(true);
    }
};

char* last_note_mask(vector<key_value> &map, char* last_note, char* mask, char* timing_str) {
    strcpy(mask, "000000 : |           |           |           |           |           |           |");

    vector<int> note_nums;
    vector<int> continue_nums;
    char buf[8];
    int offset = NOTE_NUM_OFFSET - MIN_NOTE_POS;
    // last notes:
    for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
        if ( last_note[i] == 'o' ) {
            note_nums.push_back(i + offset);
        }
        if ( last_note[i] == '+' ) {
            continue_nums.push_back(i + offset);
        }
    }
    bool time_ristriction = true;
    if ( timing_str == NULL ) {
        time_ristriction = false;
    }
    for (int i = 0; i < map.size(); i++) {
        if ( time_ristriction && NULL == strstr(map[i].key, timing_str) ) continue;
        
        key_contents kc;
        kc.init(map[i].key);

        // search notes and continues in prev zone:
        bool found = false;
        for (int j = 0; j < note_nums.size(); j++) {
            for (int k = 0; k < kc.prev_on.size(); k++) {
                if ( note_nums[j] == kc.prev_on[k] ) {
                    found = true;
                    break;
                }
            }
            if ( found ) break;
        }
        if ( !found ) {
            for (int j = 0; j < continue_nums.size(); j++) {
                for (int k = 0; k < kc.prev_continue.size(); k++) {
                    if ( continue_nums[j] == kc.prev_continue[k] ) {
                        found = true;
                        break;
                    }
                }
                if ( found ) break;
            }
        }
        
        if ( found ) {
            for (int k = 0; k < kc.cur_on.size(); k++) {
                int possible_note_pos = kc.cur_on[k] - offset;
                if ( mask[possible_note_pos] == 'x' ) continue;
                if ( mask[possible_note_pos] == '+' ) {
                    mask[possible_note_pos] = 'x';
                } else {
                    mask[possible_note_pos] = 'o';
                }
            }
            for (int k = 0; k < kc.cur_continue.size(); k++) {
                int possible_note_pos = kc.cur_continue[k] - offset;
                if ( mask[possible_note_pos] == 'x' ) continue;
                if ( mask[possible_note_pos] == 'o' ) {
                    mask[possible_note_pos] = 'x';
                } else {
                    mask[possible_note_pos] = '+';
                }
            }
        }        
    }
    return (mask);
}

char* voice_note_mask(vector<key_value> &map, char* mask) {
    char v[3];
    strcpy(v, "0c");
    char v_str[16];
    strcpy(v_str, "cvdwefxgyazb");
    char oct_str[8];
    strcpy(oct_str, "0123456");
    
    strcpy(mask, "000000 : |           |           |           |           |           |           |");

    for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
        v[0] = oct_str[(i - MIN_NOTE_POS) / 12];
        v[1] = v_str[(i - MIN_NOTE_POS) % 12];
        bool found = false;
        for (int mi = 0; mi < map.size(); mi++) {
            if ( strstr(map[mi].key, v) != NULL ) {
                found = true;
                break;
            }
        }
        if ( found ) {
            mask[i] = 'x';
        }
    }
    
    return mask;
}

char* last_note_mask2(vector<key_value> &map, char* last_note, char* mask, char* timing_str) {
    strcpy(mask, "000000 : |           |           |           |           |           |           |");
    float mask_note_val[128] = {0};
    float mask_continue_val[128] = {0};

    vector<int> note_nums;
    vector<int> continue_nums;
    char buf[8];
    int offset = NOTE_NUM_OFFSET - MIN_NOTE_POS;
    // last notes:
    for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
        if ( last_note[i] == 'o' ) {
            note_nums.push_back(i + offset);
        }
        if ( last_note[i] == '+' ) {
            continue_nums.push_back(i + offset);
        }
    }
    bool time_ristriction = true;
    if ( timing_str == NULL ) {
        time_ristriction = false;
    }
    for (int i = 0; i < map.size(); i++) {
        if ( time_ristriction && NULL == strstr(map[i].key, timing_str) ) continue;
        
        key_contents kc;
        kc.init(map[i].key);

        // search notes and continues in prev zone:
        bool found = false;
        for (int j = 0; j < note_nums.size(); j++) {
            for (int k = 0; k < kc.prev_on.size(); k++) {
                if ( note_nums[j] == kc.prev_on[k] ) {
                    found = true;
                    break;
                }
            }
            if ( found ) break;
        }
        if ( !found ) {
            for (int j = 0; j < continue_nums.size(); j++) {
                for (int k = 0; k < kc.prev_continue.size(); k++) {
                    if ( continue_nums[j] == kc.prev_continue[k] ) {
                        found = true;
                        break;
                    }
                }
                if ( found ) break;
            }
        }
        
        if ( found ) {
            for (int k = 0; k < kc.cur_on.size(); k++) {
                int possible_note_pos = kc.cur_on[k] - offset;
                mask_note_val[possible_note_pos] += map[i].val;
            }
            for (int k = 0; k < kc.cur_continue.size(); k++) {
                int possible_note_pos = kc.cur_continue[k] - offset;
                bool can_continue = false;
                for (int j = 0; j < note_nums.size(); j++) {
                    if (note_nums[j] == possible_note_pos + offset) can_continue = true;
                }
                for (int j = 0; j < continue_nums.size(); j++) {
                    if (continue_nums[j] == possible_note_pos + offset) can_continue = true;
                }
                if ( can_continue ) {
                    mask_continue_val[possible_note_pos] += map[i].val;
                }
            }
        }
    }
    for (int cnt = 0; cnt < max_masks; cnt++) {
        float max_val = 0;
        int found = 0;
        for (int i = 0; i < 128; i++) {
            if ( max_val < mask_note_val[i] ) {
                found = i;
                max_val = mask_note_val[i];
            }
            if ( max_val < mask_continue_val[i] ) {
                found = i + 128;
                max_val = mask_continue_val[i];
            }
        }
        if ( found ) {
            if ( found >= 128 ) { // continue:
                found -= 128;
                if ( mask[found] == 'o' ) {
                    mask[found] = 'x';
                } else {
                    mask[found] = '+';
                }
                mask_continue_val[found] = 0;
            } else { // note:
                if ( mask[found] == '+' ) {
                    mask[found] = 'x';
                } else {
                    mask[found] = 'o';
                }
                mask_note_val[found] = 0;
            }
        }
    }
    return (mask);
}

float make_new_frame(
        notes &new_note, 
        notes &last_note, 
        vector<key_value> &map,
        vector<key_value> &ref, 
        char* mask,
        correlator &corr,
        float cur_xcor, 
        bool verbose
) 
{
    clock_t start, end, st, ed;
    
    float best_xcor = 0.0;
    vector<key_value> max_items_add;
    vector<key_value> items_add;
    vector<key_value> items_tmp;
    new_note.time = last_note.time + 240;
    vector <notes> new_notes;
    new_notes.clear();
    notes n;
    n.time = 0;
    new_notes.push_back(n);
    
    char ln_mask[256];
    strcpy(ln_mask, mask);
    for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
        if ( ln_mask[i] == 'x' && !is_note_ch(last_note.note[i]) ) {
            ln_mask[i] = 'o';
        }
    }
    if ( verbose ) printf("\e[34m%s\e[m\n", ln_mask);
    
    note_generator ng;
    ng.init(last_note, ln_mask);
    int cnt = 0;

    st = clock();
    start = clock();

    while (!ng.completed) {
        
        ng.get_note();
        if ( !ng.maskChk() || !ng.continueChk() ) continue;

        end = clock();
        generate_time += end - start;
        start = end;

        strcpy(new_notes[0].note, ng.buf);
        note2vmap(new_notes, items_add);
        mapcopy(items_tmp, map);
        add_map_insert(items_tmp, items_add);

        end = clock();
        add_map_time += end - start;
        start = end;

        float xcor = corr.calc_if_a(items_tmp, ref);
        //float xcor = correlation(items_tmp, ref);

        end = clock();
        correlation_time += end - start;
        start = end;

        //if ( verbose ) printf("\e[33m%s - %.8f\e[m\r", ng.buf, xcor);
        if ( xcor > best_xcor ) {
            if ( verbose ) {
                if ( xcor <= cur_xcor )
                    printf("\e[31m%s - %.8f\e[m\r", ng.buf, xcor);
                else
                    printf("\e[32m%s - %.8f\e[m\r", ng.buf, xcor);
            }
            best_xcor = xcor;
            strcpy(new_note.note, ng.buf);
            mapcopy(max_items_add, items_add);
        }
    }
    
    ed = clock();
    while_time += ed - st;
        
    if ( continue_if_minus && best_xcor < cur_xcor ) {
        // just continue previous notes when cannot gain the xcor.
        char cntn_note_str[128];
        if ( ( (last_note.time + 240) % 960) == 0 ) {
            sprintf(cntn_note_str, "%06d : |-----------|-----------|-----------|-----------|-----------|-----------|", last_note.time + 240);
        } else {
            sprintf(cntn_note_str, "%06d : |           |           |           |           |           |           |", last_note.time + 240);
        }
        for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
            if ( is_note_ch(last_note.note[i]) ) cntn_note_str[i] = '+';
        }
        strcpy(new_notes[0].note, cntn_note_str);
        note2vmap(new_notes, items_add);        
        mapcopy(items_tmp, map);
        add_map_insert(items_tmp, items_add);
        best_xcor = corr.calc_if_a(items_tmp, ref);        
        strcpy(new_note.note, cntn_note_str);
        mapcopy(max_items_add, items_add);
    }
    add_map_insert(map, max_items_add);
    float re_xcor = corr.init(map, ref);
    //float re_xcor = correlation(map, ref);
    //printf("best:%.8f - re-calc:%.8f\n", best_xcor, re_xcor);
    return(re_xcor);
}



int main(int argc, char *argv[])
{
    if ( argc < 3 ) {
        printf("usage : add_frames [note file] [reference reduced vmap file] [frames to add(default:16)] options\n");
        exit(1);
    }
    int num_frames = 16;
    if ( argc >= 4 ) {
        num_frames = atoi(argv[3]);
    }
    bool verbose = false;
    if ( argc == 5 && strstr(argv[4], "v") ) {
        verbose = true;
    }
    if ( argc == 5 && strstr(argv[4], "c") ) {
        continue_if_minus = true;
    }
    bool reduced = true;
    
    if (verbose) {
        printf("seed file:%s\n", argv[1]);
        printf("vmap file:%s\n", argv[2]);
        printf("frames to make:%d\n", num_frames);
        printf("note generators:%d\n", MAX_NOTES_GENERATOR);
        if ( continue_if_minus ) printf("continue if minus:true\n");
        else printf("continue if minus:false\n");
        printf("CHANGE LOG:----\n");
        printf("ver.2015-09-08:Branched from add_frames\n");
    }
    
    // load src notes:
    int src_len = load_note_file(argv[1], cur_notes);
    
    // note2map
    note2vmap(cur_notes, cur_map);
    
    // map2reduce
    int map_len = map2reduce(cur_map);
    
    // load reference:
    load_key_value_file(argv[2], ref_map, reduced);

    // make note mask:
    char mask[256];
    char allnotes[256];
    voice_note_mask(ref_map, mask);
    if ( verbose ) {
        printf("-- all note mask --\n%s\n---------------\n", mask);
    }

    for (int i = 0; i < cur_notes.size() - 1; i++) {
        printf("%s\n", cur_notes[i].note);
    }
    
    notes new_note;
    notes last_note = cur_notes.back();
    correlator corr;
    float last_xcor = corr.init(cur_map, ref_map);
    //float last_xcor = correlation(cur_map, ref_map);
    float start_xcor = last_xcor;
    if ( verbose ) printf("%s - %.8f\n", last_note.note, last_xcor);
    else printf("%s\n", last_note.note);
    
    add_map_time = (clock_t)0;
    correlation_time = (clock_t)0;
    while_time = (clock_t)0;
    generate_time = (clock_t)0;
    
    // new frames:
    for (int i = 0; i < num_frames; i++ ) {
        float xcor = make_new_frame(
            new_note, 
            last_note, 
            cur_map, 
            ref_map, 
            mask,
            corr,
            last_xcor,
            verbose
        );
        if ( verbose ) {
            if ( xcor < last_xcor )
                printf("\e[31m%s - %.8f\e[m %3d/%3d\n", new_note.note, xcor, i, num_frames);
            else
                printf("%s - %.8f %3d/%3d\n", new_note.note, xcor, i, num_frames);
        }
        else printf("%s\n", new_note.note);
        last_xcor = xcor;
        cur_notes.push_back(new_note);
        last_note = cur_notes.back();
    }
    printf ("add map time: %.2f s / correlation time: %.2f s\n", (double)add_map_time/CLOCKS_PER_SEC, (double)correlation_time/CLOCKS_PER_SEC);
    printf ("generate time: %.2f / while time: %.2f s\n", (double)generate_time/CLOCKS_PER_SEC, (double)while_time/CLOCKS_PER_SEC);
    if ( verbose ) {
        printf ("Result notes:---------------\n");
        for (int i = 0; i < cur_notes.size(); i++) {
            printf("%s\n", cur_notes[i].note);
        }
    }
}