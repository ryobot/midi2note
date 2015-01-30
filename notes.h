// notes.h
#ifndef NOTES_H
#define NOTES_H

#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <string.h>
using namespace std;

struct notes {
    int time;
    char note[128];
    
    void init (char* buf);
};

int load_note_file(char* filename, vector<notes> &items);

#endif