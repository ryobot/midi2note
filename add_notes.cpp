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

#define MAX_NOTES_GENERATOR 4
//#define MAX_NOTE_POS_TMP 20

struct note_generator {
    int num_notes;
    int index[MAX_NOTES_GENERATOR];
    char type[MAX_NOTES_GENERATOR];
    char buf[128];
    char ref[128];
    char mask[128];
    bool completed;
    int beat_pos;
    
    void init(notes &last_note, char* _mask) {
        strcpy(mask, _mask);
        for(int i = 0; i < MAX_NOTES_GENERATOR; i++) {
            index[i] = MIN_NOTE_POS + i*12;
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
        if ( index[in] > MAX_NOTE_POS ) {
            if ( in > 0 ) {
                iterate(in - 1);
                index[in] = index[in - 1] + 1;
                if ( index[in] < MIN_NOTE_POS + in*12 ) index[in] = MIN_NOTE_POS + in*12;
            } else {
                completed = true;
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
            for(int j = 0; j < MAX_NOTES_GENERATOR; j++) {
                if ( index[j] == i ) {
                    buf[i] = type[j];
                    break;
                }
            }
        }
        iterate(MAX_NOTES_GENERATOR - 1);
        return buf;
    }
    bool maskChk() {
        for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
            if ( is_note_ch(buf[i]) && mask[i] != 'o' ) {
                return(false);
            }
        }
        return(true);
    }
};

float make_new_frame(notes &new_note, notes &last_note, vector<key_value> &map, vector<key_value> &ref, char* mask, float cur_xcor) {
    //float cur_xcor = correlation(map, ref);
    //printf("cur xcor : %.4f\n", cur_xcor);
    float max_xcor = 0.0;
    vector<key_value> max_items_add;
    vector<key_value> items_add;
    vector<key_value> items_tmp;
    var_data lastVar, newVar;
    make_vars(last_note.note, lastVar);
    new_note.time = last_note.time + 240;
    char timing_str[16];
    int timing_res = 4;
    if ( timing_res ) {
        int timing = (new_note.time / 240) % timing_res;
        sprintf(timing_str, "t%d%d", timing_res, timing);
    }
    else {
        strcpy(timing_str, "t00");
    }

    note_generator ng;
    ng.init(last_note, mask);
    int cnt = 0;
    while (!ng.completed) {
        ng.get_note();
        if ( !ng.maskChk() ) continue;
        make_vars(ng.buf, newVar);
        make_maps(lastVar, newVar, items_add);
        mapcopy(items_tmp, map);
        for ( int i = 0; i < items_add.size(); i++ ) {
            strcat(items_add[i].key, timing_str);
        }
        add_map(items_tmp, items_add);
        float xcor = correlation(items_tmp, ref);
        if ( xcor > max_xcor ) {
            //printf("%s - %.4f\n", ng.buf, xcor);
            max_xcor = xcor;
            strcpy(new_note.note, ng.buf);
            mapcopy(max_items_add, items_add);
        }
        //if ( cnt++ > 100 ) break;
    }
    add_map(map, max_items_add);
    return(max_xcor);
}

int main(int argc, char *argv[])
{
    if ( argc < 3 ) {
        printf("usage : add_notes [note file] [reference reduced map file] [note mask file] [frames to add(default:16)]\n");
        exit(1);
    }
    int num_frames = 16;
    if ( argc >= 5 ) {
        num_frames = atoi(argv[4]);
    }
    bool verbose = false;
    if ( argc == 6 && strcmp(argv[5], "-v") == 0 ) {
        verbose = true;
    }
    bool reduced = true;
    
    // load src notes:
    int src_len = load_note_file(argv[1], cur_notes);
    
    // load note mask:
    char mask[256];
    load_mask_file(argv[3], mask);
    if ( verbose ) {
        printf("-- note mask --\n%s\n---------------\n", mask);
    }
    // note2map
    note2map(cur_notes, cur_map);
    
    // map2reduce
    int map_len = map2reduce(cur_map);
    
    // load reference:
    load_key_value_file(argv[2], ref_map, reduced);

    for (int i = 0; i < cur_notes.size() - 1; i++) {
        printf("%s\n", cur_notes[i].note);
    }
    
    notes new_note;
    notes last_note = cur_notes.back();
    float xcor = correlation(cur_map, ref_map);
    if ( verbose ) printf("%s - %.4f\n", last_note.note, xcor);
    else printf("%s\n", last_note.note);
    // new frames:
    for (int i = 0; i < num_frames; i++ ) {
        xcor = make_new_frame(new_note, last_note, cur_map, ref_map, mask, xcor);
        if ( verbose ) printf("%s - %.4f\n", new_note.note, xcor);
        else printf("%s\n", new_note.note);
        cur_notes.push_back(new_note);
        last_note = cur_notes.back();
    }
}