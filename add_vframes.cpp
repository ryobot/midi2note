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

bool continue_if_minus = false;
int max_masks = MAX_MASKS;

clock_t add_map_time, correlation_time, generate_time, while_time, recalc_time, mask_time;

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
    start = clock();
    
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

        strcpy(new_notes[0].note, ng.buf);
        note2vmap(new_notes, items_add);
        mapcopy(items_tmp, map);
        add_map_insert(items_tmp, items_add);

        end = clock();
        add_map_time += end - start;
        start = end;

        float xcor = corr.calc_if_a(items_tmp, ref);

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

    end = clock();
    recalc_time += end - start;
    start = end;

    printf("best:%.8f - re-calc:%.8f\n", best_xcor, re_xcor);
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
    recalc_time = (clock_t)0;
    mask_time = (clock_t)0;
    
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
    printf ("mask time: %.2f s / re-calc time: %.2f s\n", (double)mask_time/CLOCKS_PER_SEC, (double)recalc_time/CLOCKS_PER_SEC);
    if ( verbose ) {
        printf ("Result notes:---------------\n");
        for (int i = 0; i < cur_notes.size(); i++) {
            printf("%s\n", cur_notes[i].note);
        }
    }
}