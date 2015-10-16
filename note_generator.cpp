// note_generator.cpp

#include "note_generator.h"

void note_generator::init(notes &last_note, char* _mask, int note_generators, bool _include_none) {
    include_none = _include_none;
    strcpy(mask, _mask);
    generators_num = note_generators;
    for(int i = 0; i < generators_num; i++) {
        index[i] = MIN_NOTE_POS + i*INIT_NOTES_SEPARATION;
        type[i] = 'o';
    }
    strcpy(ref, last_note.note);
    beat_pos = (last_note.time + 240) % 960;
    if ( beat_pos == 0 ) {
        sprintf(init_buf, "%06d : |-----------|-----------|-----------|-----------|-----------|-----------|", last_note.time + 240);
    } else {
        sprintf(init_buf, "%06d : |           |           |           |           |           |           |", last_note.time + 240);
    }
    strcpy(buf, init_buf);
    completed = false;
}

void note_generator::iterate(int in) {
    if (in < 0) {
        completed = true;
        return;
    }
    if ( is_note_ch(ref[index[in]]) && type[in] == 'o' ) {
        type[in] = '+';
    } else {
        index[in]++;
        while( !is_note_ch(mask[index[in]]) && index[in] < MAX_NOTE_POS ) index[in]++;
        type[in] = 'o';
    }
    if ( index[in] >= MAX_NOTE_POS ) {
        if ( in > 0 ) {
            iterate(in - 1);
            index[in] = index[in - 1] + 1;
            if ( index[in] < MIN_NOTE_POS + in*INIT_NOTES_SEPARATION ) index[in] = MIN_NOTE_POS + in*INIT_NOTES_SEPARATION;
        } else {
            if ( --generators_num <= 0 ) {
                if ( include_none ) {
                    include_none = false;
                } else {
                    completed = true;
                }
            } else {
                for(int i = 0; i < generators_num; i++) {
                    index[i] = MIN_NOTE_POS + i*INIT_NOTES_SEPARATION;
                    type[i] = 'o';
                }
            }
        }
    }
}

char* note_generator::get_note() {
    strcpy(buf, init_buf);
    for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
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

bool note_generator::maskChk() {
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

bool note_generator::continueChk() {
    for (int i = MIN_NOTE_POS; i < MAX_NOTE_POS; i++ ) {
        if ( buf[i] == '+' && !is_note_ch(ref[i]) ) {
            return(false);
        }
    }
    return(true);
}
