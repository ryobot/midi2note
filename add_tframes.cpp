// add_notes.cpp
//#include <typeinfo>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <time.h>
using namespace std;

#include "note_map.h"
#include "note_generator.h"

vector<key_value> ref_map;
vector<key_value> cur_map;
vector<notes> cur_notes;
vector<notes> org_notes;

clock_t copy_map_time,  add_map_time, make_map_time, correlation_time, generate_time, while_time, recalc_time, mask_time, total_time;
clock_t start, end;

//bool continue_if_minus = false;
int max_masks = MAX_MASKS;
int mix = 0;
bool acceptNoNotesMix = true;

int valueMode = VALUE_BY_NOTE_NUM;

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

char* last_note_mask2(vector<key_value> &map, char* last_note, char* mask, char* timing_str, vector<key_value> &mask_map) {
    strcpy(mask, "000000 : |           |           |           |           |           |           |");
    float mask_note_val[128] = {0};
    float mask_continue_val[128] = {0};
    mask_map.clear();

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
            mask_map.push_back(map[i]);
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

struct keyData {
    char str[64];
};

char* last_note_mask3(vector<key_value> &map, var_data &last_var, char* mask, char* timing_str, vector<key_value> &mask_map) {
    strcpy(mask, "000000 : |           |           |           |           |           |           |");
    float mask_note_val[128] = {0};
    float mask_continue_val[128] = {0};
    mask_map.clear();

    bool time_ristriction = true;
    if ( timing_str == NULL ) {
        time_ristriction = false;
    }
    
    // Making prevKeys from last_var:
    int offset = NOTE_NUM_OFFSET - MIN_NOTE_POS;
    vector<keyData> prevKeys;
    prevKeys.clear();
    for (int v = 0; v < last_var.strs.size(); v++) {
        keyData data;
        strcpy(data.str, "");
        // previous notes:
        for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
            if ( last_var.strs[v].str[i] == 'o' ) {
                strcat(data.str, get_n_note(i + offset));
            }
            else if ( last_var.strs[v].str[i] == '+' ) {
                strcat(data.str, get_c_note(i + offset));
            }
        }
        prevKeys.push_back(data);
    }
    
    // search maps which prevkey matches:
    for (int i = 0; i < map.size(); i++) {
        if ( time_ristriction && NULL == strstr(map[i].key, timing_str) ) continue;
        
        key_contents kc;
        kc.init(map[i].key);
        
        for (int v = 0; v < prevKeys.size(); v++  ) {
            if ( strcmp(kc.prevKey, prevKeys[v].str) == 0 ) {
                mask_map.push_back(map[i]);
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
    }
    return (mask);
}

float make_new_frame(
        notes &new_note, 
        notes &last_note, 
        vector<key_value> &map,
        vector<key_value> &ref, 
        correlator &corr,
        float cur_xcor, 
        char* mask, 
        int generators,
        bool verbose
) 
{
    clock_t start, end, st, ed;
    start = clock();
    
    float best_xcor = 0.0;
    vector<key_value> max_items_add;
    vector<key_value> items_add;
    vector<key_value> items_tmp;
    vector<key_value> mask_map;
    //key_value_hash kvh;
    var_data lastVar, newVar;
    make_vars(last_note.note, lastVar);
    new_note.time = last_note.time + 240;
    
    float min_diff = 1.0;
    
    char timing_str[16];
    int timing_res = 4;
    if ( timing_res ) {
        int timing = (new_note.time / 240) % timing_res;
        sprintf(timing_str, "t%d%d", timing_res, timing);
    }
    else {
        strcpy(timing_str, "t00");
    }
        
    char ln_mask[256];
    if ( mask != NULL ) {
        strcpy(ln_mask, mask);
    } else {
        last_note_mask3(ref, lastVar, ln_mask, timing_str, mask_map);
    }
    //if ( verbose ) printf("\e[34m%s > %.8f map length %d\e[m\n", ln_mask, target_xcor, mask_map.size());
    
    note_generator ng;
    ng.init(last_note, ln_mask, generators);
    int cnt = 0;

    end = clock();
    mask_time += end - start;
    start = end;
    st = end;

    while (!ng.completed) {
        
        ng.get_note();
        if ( !ng.maskChk() || !ng.continueChk() ) continue;
        
        end = clock();
        generate_time += end - start;
        start = end;

        make_vars(ng.buf, newVar);
        make_maps(lastVar, newVar, items_add, valueMode);

        end = clock();
        make_map_time += end - start;
        start = end;

        mapcopy(items_tmp, map);
        for ( int i = 0; i < items_add.size(); i++ ) {
            strcat(items_add[i].key, timing_str);
        }
        
        end = clock();
        copy_map_time += end - start;
        start = end;

        add_map_insert(items_tmp, items_add);

        end = clock();
        add_map_time += end - start;
        start = end;

        float xcor = corr.calc_if_a(items_tmp, ref);
        
        end = clock();
        correlation_time += end - start;
        start = end;

        if ( xcor > best_xcor ) {
            //if ( verbose ) {
            //    if ( xcor < cur_xcor )
            //        printf("\e[31m%s - %.8f\e[m\r", ng.buf, xcor);
            //    else
            //        printf("\e[32m%s - %.8f\e[m\r", ng.buf, xcor);
            //}
            best_xcor = xcor;
            strcpy(new_note.note, ng.buf);
            mapcopy(max_items_add, items_add);
        }
    }
    
    ed = clock();
    while_time += ed - st;

    /*
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
        make_vars(cntn_note_str, newVar);
        make_maps(lastVar, newVar, items_add);
        mapcopy(items_tmp, map);
        for ( int i = 0; i < items_add.size(); i++ ) {
            strcat(items_add[i].key, timing_str);
        }
        add_map_insert(items_tmp, items_add);
        best_xcor = corr.calc_if_a(items_tmp, ref);        
        strcpy(new_note.note, cntn_note_str);
        mapcopy(max_items_add, items_add);
    }*/
    add_map_insert(map, max_items_add);
    map_sort(map);
    float re_xcor = corr.init(map, ref);
    
    end = clock();
    recalc_time += end - start;
    start = end;

    //printf("best:%.8f - re-calc:%.8f\n", best_xcor, re_xcor);
    return(re_xcor);
}

char* mask_mix(char* mask, char* org_note) {
    for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
        if ( is_note_ch(org_note[i]) ) {
            if ( ( i - MIN_NOTE_POS ) % 12 == 0 ) mask[i] = '|';
            else mask[i] = ' ';
        }
    }
}

char* unmask_mix(char* mask, char* org_note) {
    for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
        if ( is_note_ch(org_note[i]) ) {
            mask[i] = org_note[i];
        }
    }
}

float mix_new_frame(
        notes &new_note, 
        notes &last_note, 
        vector<key_value> &map,
        vector<key_value> &ref, 
        correlator &corr,
        float cur_xcor, 
        char* mask, 
        int generators,
        bool verbose
) 
{
    clock_t start, end, st, ed;
    start = clock();
    
    //float cur_xcor = correlation(map, ref);
    //printf("cur xcor : %.4f\n", cur_xcor);
    float best_xcor = 0.0;
    vector<key_value> max_items_add;
    vector<key_value> items_add;
    vector<key_value> items_tmp;
    vector<key_value> mask_map;
    //key_value_hash kvh;
    var_data lastVar, newVar;
    make_vars(last_note.note, lastVar);
    //new_note.time = last_note.time + 240;
    
    float min_diff = 1.0;
    
    char timing_str[16];
    int timing_res = 4;
    if ( timing_res ) {
        int timing = (new_note.time / 240) % timing_res;
        sprintf(timing_str, "t%d%d", timing_res, timing);
    }
    else {
        strcpy(timing_str, "t00");
    }
        
    char ln_mask[256];
    char mixed[256];
    char max_mixed[256];
    if ( mask != NULL ) {
        strcpy(ln_mask, mask);
    } else {
        //last_note_mask2(ref, last_note.note, ln_mask, timing_str, mask_map);
        last_note_mask3(ref, lastVar, ln_mask, timing_str, mask_map);
    }
    mask_mix(ln_mask, new_note.note);
    note_generator ng;
    // Including no add notes in mixing:
    bool include_none = true;
    if ( !acceptNoNotesMix ) {
        include_none = false;
    }
    ng.init(last_note, ln_mask, generators, include_none);
    int cnt = 0;

    end = clock();
    mask_time += end - start;
    start = end;
    st = end;

    while (!ng.completed) {
        
        ng.get_note();
        if ( !ng.maskChk() || !ng.continueChk() ) continue;
        
        end = clock();
        generate_time += end - start;
        start = end;

        strcpy(mixed, ng.buf);
        unmask_mix(mixed, new_note.note);
        make_vars(mixed, newVar);
        make_maps(lastVar, newVar, items_add, valueMode);

        end = clock();
        make_map_time += end - start;
        start = end;

        mapcopy(items_tmp, map);
        for ( int i = 0; i < items_add.size(); i++ ) {
            strcat(items_add[i].key, timing_str);
        }
        
        end = clock();
        copy_map_time += end - start;
        start = end;

        add_map_insert(items_tmp, items_add);

        end = clock();
        add_map_time += end - start;
        start = end;

        float xcor = corr.calc_if_a(items_tmp, ref);
        
        end = clock();
        correlation_time += end - start;
        start = end;

        if ( xcor > best_xcor ) {
            //if ( verbose ) {
            //    if ( xcor < cur_xcor )
            //        printf("\e[31m%s - %.8f\e[m\r", ng.buf, xcor);
            //    else
            //        printf("\e[32m%s - %.8f\e[m\r", ng.buf, xcor);
            //}
            best_xcor = xcor;
            strcpy(max_mixed, mixed);
            mapcopy(max_items_add, items_add);
        }
    }
    
    ed = clock();
    while_time += ed - st;

    /*
    if ( best_xcor < cur_xcor ) {
        // do not add notes when cannot gain the xcor.
        char cntn_note_str[128];
        if ( ( (last_note.time + 240) % 960) == 0 ) {
            sprintf(cntn_note_str, "%06d : |-----------|-----------|-----------|-----------|-----------|-----------|", last_note.time + 240);
        } else {
            sprintf(cntn_note_str, "%06d : |           |           |           |           |           |           |", last_note.time + 240);
        }
        for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
            if ( is_note_ch(last_note.note[i]) ) cntn_note_str[i] = '+';
        }
        make_vars(cntn_note_str, newVar);
        make_maps(lastVar, newVar, items_add);
        mapcopy(items_tmp, map);
        for ( int i = 0; i < items_add.size(); i++ ) {
            strcat(items_add[i].key, timing_str);
        }
        add_map_insert(items_tmp, items_add);
        best_xcor = corr.calc_if_a(items_tmp, ref);        
        strcpy(max_mixed, cntn_note_str);
        mapcopy(max_items_add, items_add);
    }*/
    strcpy(new_note.note, max_mixed);
    add_map_insert(map, max_items_add);
    map_sort(map);
    float re_xcor = corr.init(map, ref);
    
    end = clock();
    recalc_time += end - start;
    start = end;

    //printf("best:%.8f - re-calc:%.8f\n", best_xcor, re_xcor);
    return(re_xcor);
}

int main(int argc, char *argv[])
{
    // initializations:
    setvbuf(stdout, (char *)NULL, _IONBF, 0);
    init_note_num();
    
    if ( argc < 3 ) {
        printf("usage : add_frames [note file] [reference reduced map file] [frames to add(default:16)] options\n");
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
    //if ( argc == 5 && strstr(argv[4], "c") ) {
    //    continue_if_minus = true;
    //}
    if ( argc == 5 && strstr(argv[4], "h") ) {
        max_masks = 16;
        if ( strstr(argv[4], "h20") ) {
            max_masks = 20;
        }
        if ( strstr(argv[4], "h24") ) {
            max_masks = 24;
        }
        if ( strstr(argv[4], "hx") ) {
            max_masks = 73;
        }
    }
    int generators = 1;
    if ( argc == 5 && strstr(argv[4], "g") ) {
        for (int i = 1; i <= 9; i++) {
            char num_str[8];
            sprintf(num_str, "g%d", i);
            if ( strstr(argv[4], num_str) ) {
                generators = i;
                break;
            }
        }
    }
    int generators_decrement = 0;
    if ( argc == 5 && strstr(argv[4], "d") ) {
        for (int i = 1; i <= 9; i++) {
            char num_str[8];
            sprintf(num_str, "d%d", i);
            if ( strstr(argv[4], num_str) ) {
                generators_decrement = i;
                break;
            }
        }
    }
    int num_mix = 1;
    if ( argc == 5 && strstr(argv[4], "m") ) {
        for (int i = 1; i <= 9; i++) {
            char num_str[8];
            sprintf(num_str, "m%d", i);
            if ( strstr(argv[4], num_str) ) {
                num_mix = i;
                break;
            }
        }
    }
    acceptNoNotesMix = true;
    if ( argc == 5 && strstr(argv[4], "n") ) {
        acceptNoNotesMix = false;
    }
    if ( argc == 5 && strstr(argv[4], "l") ) {
        if ( strstr(argv[4], "lx") ) {
            valueMode = VALUE_BY_NOTE_NUM;
        }
        if ( strstr(argv[4], "l/") ) {
            valueMode = VALUE_DIVIDED_NOTE_NUM;
        }
        if ( strstr(argv[4], "ls") ) {
            valueMode = VALUE_SAME_NOTE_NUM;
        }
    }
    bool reduced = true;
    
    if (verbose) {
        printf("seed file:%s\n", argv[1]);
        printf("map file:%s\n", argv[2]);
        printf("frames to make:%d\n", num_frames);
        printf("max masks:%d\n", max_masks);
        printf("note generators:%d\n", generators);
        printf("generators decrement:%d\n", generators_decrement);
        printf("mixing:%d\n", num_mix);
        if ( acceptNoNotesMix ) printf("accept no note mix:true\n");
        else printf("accept no note mix:false\n");
        switch (valueMode) {
            case VALUE_BY_NOTE_NUM:
                printf("value mode: x notenum\n");
                break;
            case VALUE_DIVIDED_NOTE_NUM:
                printf("value mode: / notenum\n");
                break;
            case VALUE_SAME_NOTE_NUM:
                printf("value mode: x 1\n");
                break;
            default:
                printf("value mode: error\n");
                break;
        }
        //if ( continue_if_minus ) printf("continue if minus:true\n");
        //else printf("continue if minus:false\n");
        printf("CHANGE LOG:----\n");
        printf("ver.2015-07-07:Values for each note transition not divided by possible transition count. (note_map.cpp)\n");
        printf("ver.2015-07-08:Just continue previous notes when cannot gain the xcor.\n");
        printf("ver.2015-07-14:Mask selected by map values top 16.\n");
        printf("ver.2015-07-15:Just continue previous notes when cannot gain the xcor - bypassed.\n");
        printf("ver.2015-07-16:Values for each note transition by number of concering notes. (note_map.cpp)\n");
        printf("ver.2015-07-17:Max masks in command ooption. h/h20/h24 \n");
        printf("ver.2015-08-17:Last note masks bug fixed. Program name changed to add_frames. \n");
        printf("ver.2015-09-15:add_tframes / Branched from add_frames \n");
        printf("ver.2015-09-xx:Faster correlation \n");
        printf("ver.2015-10-07:MIX MODE added\n");
        printf("ver.2015-10-xx:No notes added when its correlation is best in mixing.\n");
        printf("ver.2015-10-16:Generators decrement\n");
        printf("ver.2015-10-22:Reverted -> Values for each note transition by number of concering notes. (note_map.cpp)\n");
        printf("ver.2015-10-23:Values for each note transition divided by number of variations.\n");
        printf("ver.2015-11-05:Switch 'n' for bypassing 'No notes added when its correlation is best in mixing'.\n");
    }
    
    // load src notes:
    int src_len = load_note_file(argv[1], org_notes);
    
    // load reference:
    //load_key_value_file(argv[2], ref_map, reduced);
    load_note_file(argv[2], cur_notes);
    note2map(cur_notes, ref_map, valueMode, 4, false);
    map2reduce(ref_map);
    
    // make note mask:
    char mask[256];
    char allnotes[256];
    strcpy(allnotes, "000000 : o+++++++++++o+++++++++++o+++++++++++o+++++++++++o+++++++++++o+++++++++++o");
    last_note_mask(ref_map, allnotes, mask, NULL);
    if ( verbose ) {
        printf("-- all note mask --\n%s\n", mask);
    }

    for (mix = 0; mix < num_mix; mix++) { // start of mix for loop:
    
    if ( verbose ) {
        printf("mix %d / generators %d ---------------------\n", mix, generators);
    }
    
    cur_notes.clear();
    for (int i = 0; i < src_len; i++) {
        cur_notes.push_back(org_notes[i]);
    }
    
    // note2map
    note2map(cur_notes, cur_map, valueMode);
    
    // map2reduce
    int map_len = map2reduce(cur_map);
    
    for (int i = 0; i < cur_notes.size() - 1; i++) {
        printf("%s\n", cur_notes[i].note);
    }
    
    notes new_note;
    notes last_note = cur_notes.back();
    if ( mix ) {
        new_note = org_notes[src_len];
    }
    //float last_xcor = correlation(cur_map, ref_map);
    correlator corr;
    float last_xcor = corr.init(cur_map, ref_map);
    float start_xcor = last_xcor;
    if ( verbose ) printf("%s - %.8f\n", last_note.note, last_xcor);
    else printf("%s\n", last_note.note);
    
    // new frames:
    if ( mix ) {
        for (int i = src_len; i < num_frames + src_len; i++ ) {
            new_note = org_notes[i];
            float xcor = mix_new_frame(
                new_note, 
                last_note, 
                cur_map, 
                ref_map, 
                corr,
                last_xcor,
                NULL,
                generators,
                verbose
            );
            if ( verbose ) {
                for (int c = 0; c < strlen(new_note.note); c++) {
                    if (c >= MIN_NOTE_POS &&  org_notes[i].note[c] != new_note.note[c] ) {
                        printf("\e[33m");
                        printf("%c", new_note.note[c]);
                        printf("\e[m");
                    } else {
                        printf("%c", new_note.note[c]);
                    }
                }
                if ( xcor < last_xcor )
                    printf("\e[31m - %.8f\e[m", xcor);
                else
                    printf(" - %.8f", xcor);
            }
            else printf("%s\n", new_note.note);
            last_xcor = xcor;
            cur_notes.push_back(new_note);
            last_note = cur_notes.back();

            end = clock();
            total_time = end - start;
            int total_sec = total_time/CLOCKS_PER_SEC;
            int min = (total_sec/60)%60;
            int sec = total_sec%60;
            int hour = total_sec/3600;
            printf (" %3d/%3d fr. mix%d elapsed: %d:%02d:%02d\n", i, num_frames + src_len, mix, (double)total_time/CLOCKS_PER_SEC, hour, min, sec);        
        }
    } else {
        for (int i = 0; i < num_frames; i++ ) {
            float xcor = make_new_frame(
                new_note, 
                last_note, 
                cur_map, 
                ref_map, 
                corr,
                last_xcor,
                NULL,
                generators,
                verbose
            );
            if ( verbose ) {
                if ( xcor < last_xcor )
                    printf("\e[31m%s - %.8f\e[m %3d/%3d fr. ", new_note.note, xcor, i, num_frames);
                else
                    printf("%s - %.8f %3d/%3d fr. ", new_note.note, xcor, i, num_frames);
            }
            else printf("%s\n", new_note.note);
            last_xcor = xcor;
            cur_notes.push_back(new_note);
            last_note = cur_notes.back();

            end = clock();
            total_time = end - start;
            int total_sec = total_time/CLOCKS_PER_SEC;
            int min = (total_sec/60)%60;
            int sec = total_sec%60;
            int hour = total_sec/3600;
            printf ("elapsed: %d:%02d:%02d\n", (double)total_time/CLOCKS_PER_SEC, hour, min, sec);        
        }
    }

    org_notes.clear();
    for (int i = 0; i < cur_notes.size(); i++) {
        org_notes.push_back(cur_notes[i]);
    }
    
    if ( generators_decrement ) {
        generators -= generators_decrement;
        if ( generators <= 0 ) generators = 1;
    }
    
    } // end of mix for loop:
    
    printf ("make map time: %.2f s / copy map time: %.2f s / add map time: %.2f s / correlation time: %.2f s\n", 
            (double)make_map_time/CLOCKS_PER_SEC, 
            (double)copy_map_time/CLOCKS_PER_SEC, 
            (double)add_map_time/CLOCKS_PER_SEC, 
            (double)correlation_time/CLOCKS_PER_SEC);
    printf ("generate time: %.2f s / while time: %.2f s\n", (double)generate_time/CLOCKS_PER_SEC, (double)while_time/CLOCKS_PER_SEC);
    printf ("mask time: %.2f s / re-calc time: %.2f s\n", (double)mask_time/CLOCKS_PER_SEC, (double)recalc_time/CLOCKS_PER_SEC);
    if ( verbose ) {
        printf ("Result notes:---------------\n");
        for (int i = 0; i < cur_notes.size(); i++) {
            printf("%s\n", cur_notes[i].note);
        }
    }
}