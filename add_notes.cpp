// add_notes.cpp
//#include <typeinfo>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <string.h>
using namespace std;

#include "note_map.h"

vector<key_value> ref_map;
vector<key_value> cur_map;
vector<notes> cur_notes;

#define MAX_NOTES_GENERATOR 3
#define INIT_NOTES_SEPARATION 12
#define MAX_MASKS 10

bool continue_if_minus = false;
int max_masks = MAX_MASKS;

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
    
    void init(notes &last_note, char* _mask) {
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
                mask_continue_val[possible_note_pos] += map[i].val;
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
        float cur_xcor, 
        float target_xcor, 
        char* mask, 
        bool verbose
) 
{
    //float cur_xcor = correlation(map, ref);
    //printf("cur xcor : %.4f\n", cur_xcor);
    float best_xcor = 0.0;
    vector<key_value> max_items_add;
    vector<key_value> items_add;
    vector<key_value> items_tmp;
    var_data lastVar, newVar;
    make_vars(last_note.note, lastVar);
    new_note.time = last_note.time + 240;
    
    if ( target_xcor == 0 ) {
        target_xcor = cur_xcor + 0.001;
    }
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
        //last_note_mask(ref, last_note.note, ln_mask, timing_str);
        last_note_mask2(ref, last_note.note, ln_mask, timing_str);
    }
    if ( verbose ) printf("\e[34m%s > %.8f\e[m\n", ln_mask, target_xcor);
    
    note_generator ng;
    ng.init(last_note, ln_mask);
    int cnt = 0;
    
    while (!ng.completed) {
        ng.get_note();
        if ( !ng.maskChk() || !ng.continueChk() ) continue;
        make_vars(ng.buf, newVar);
        make_maps(lastVar, newVar, items_add);
        mapcopy(items_tmp, map);
        for ( int i = 0; i < items_add.size(); i++ ) {
            strcat(items_add[i].key, timing_str);
        }
        add_map(items_tmp, items_add);
        float xcor = correlation(items_tmp, ref);
        float diff = fabs(xcor - target_xcor);
        if ( verbose ) printf("\e[33m%s - %.8f\e[m\r", ng.buf, xcor);
        //if ( xcor >= max_xcor ) {
        
        //if ( (xcor < cur_xcor && xcor > best_xcor) || 
        //      (xcor >= cur_xcor && min_diff >= diff) ) {
        
        if ( xcor > best_xcor ) {
            if ( verbose ) {
                if ( xcor < cur_xcor )
                    printf("\e[31m%s - %.8f\e[m\r", ng.buf, xcor);
                else
                    printf("\e[32m%s - %.8f\e[m\r", ng.buf, xcor);
            }
            min_diff = diff;
            best_xcor = xcor;
            strcpy(new_note.note, ng.buf);
            mapcopy(max_items_add, items_add);
        }
    }
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
        add_map(items_tmp, items_add);
        best_xcor = correlation(items_tmp, ref);        
        strcpy(new_note.note, cntn_note_str);
        mapcopy(max_items_add, items_add);
    }
    add_map(map, max_items_add);
    return(best_xcor);
}

int main(int argc, char *argv[])
{
    if ( argc < 3 ) {
        printf("usage : add_notes [note file] [reference reduced map file] [frames to add(default:16)]\n");
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
    if ( argc == 5 && strstr(argv[4], "h") ) {
        max_masks = 16;
        if ( strstr(argv[4], "h20") ) {
            max_masks = 20;
            if ( strstr(argv[4], "h24") ) {
                max_masks = 24;
            }
        }
    }
    bool reduced = true;
    
    if (verbose) {
        printf("seed file:%s\n", argv[1]);
        printf("map file:%s\n", argv[2]);
        printf("frames to make:%d\n", num_frames);
        printf("max masks:%d\n", max_masks);
        printf("note generators:%d\n", MAX_NOTES_GENERATOR);
        if ( continue_if_minus ) printf("continue if minus:true\n");
        else printf("continue if minus:false\n");
        printf("CHANGE LOG:----\n");
        printf("ver.2015-07-07:Values for each note transition not divided by possible transition count. (note_map.cpp)\n");
        printf("ver.2015-07-08:Just continue previous notes when cannot gain the xcor.\n");
        printf("ver.2015-07-14:Mask selected by map values top 16.\n");
        printf("ver.2015-07-15:Just continue previous notes when cannot gain the xcor - bypassed.\n");
        printf("ver.2015-07-16:Values for each note transition by number of concering notes. (note_map.cpp)\n");
        printf("ver.2015-07-17:Max masks in command ooption. h/h20/h24 \n");
    }
    
    // load src notes:
    int src_len = load_note_file(argv[1], cur_notes);
    
    // note2map
    note2map(cur_notes, cur_map);
    
    // map2reduce
    int map_len = map2reduce(cur_map);
    
    // load reference:
    load_key_value_file(argv[2], ref_map, reduced);

    // make note mask:
    char mask[256];
    char allnotes[256];
    strcpy(allnotes, "000000 : o+++++++++++o+++++++++++o+++++++++++o+++++++++++o+++++++++++o+++++++++++o");
    last_note_mask(ref_map, allnotes, mask, NULL);
    if ( verbose ) {
        printf("-- all note mask --\n%s\n---------------\n", mask);
    }

    for (int i = 0; i < cur_notes.size() - 1; i++) {
        printf("%s\n", cur_notes[i].note);
    }
    
    notes new_note;
    notes last_note = cur_notes.back();
    float last_xcor = correlation(cur_map, ref_map);
    float start_xcor = last_xcor;
    if ( verbose ) printf("%s - %.8f\n", last_note.note, last_xcor);
    else printf("%s\n", last_note.note);
    // new frames:
    for (int i = 0; i < num_frames; i++ ) {
        float target_xcor = last_xcor + (1.0 - last_xcor) / (float)(num_frames - i);
        float target_xcor_liner = start_xcor + (1.0 - start_xcor)*(float)(i+1)/(float)num_frames;
        if ( target_xcor < target_xcor_liner ) {
            target_xcor = target_xcor_liner;
        }
        float xcor = make_new_frame(
            new_note, 
            last_note, 
            cur_map, 
            ref_map, 
            last_xcor,
            target_xcor, 
            NULL,
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
    if ( verbose ) {
        printf ("Result notes:---------------\n");
        for (int i = 0; i < cur_notes.size(); i++) {
            printf("%s\n", cur_notes[i].note);
        }
    }
}