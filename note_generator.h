// note_generator.h
#ifndef NOTE_GENERATOR_H
#define NOTE_GENERATOR_H

#include <cstdlib>
#include <vector>
#include <string>
#include <stdio.h>
#include <string.h>
using namespace std;

#include "notes.h"
#include "note_map.h"

#define MAX_NOTES_GENERATOR 10
#define INIT_NOTES_SEPARATION 6
#define MAX_MASKS 10

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
    
    void init(notes &last_note, char* _mask, int note_generators);
    void iterate(int in);
    char* get_note();
    bool maskChk();
    bool continueChk();
};

#endif
