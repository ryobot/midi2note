// midi2note.cpp
//#include <typeinfo>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <string.h>
using namespace std;

#include "note_map.h"
vector<notes> notes;

int main(int argc, char *argv[])
{
    int gates[128] = {0};
    if ( argc <= 1 ) {
        printf("usage : note2dump [txt note file]\n");
        exit(1);
    }
    load_note_file(argv[1], notes);
    for (int i = 0; i < notes.size(); i++ ) {
        int pedal = 0;
        for (int pos = MIN_NOTE_POS; pos <= MAX_NOTE_POS; pos++) {
            if ( notes[i].note[pos] == 'o' ) {
                gates[pos] = 1;
                if ( pedal == 0 ) pedal = gates[pos];
            } else if ( notes[i].note[pos] == '+' ) {
                gates[pos]++;
                if ( pedal == 0 ) pedal = gates[pos];
            } else {
                if ( gates[pos] && gates[pos] < 4 && gates[pos] <= pedal ) {
                    notes[i].note[pos] = '+';
                    gates[pos]++;
                } else {
                    gates[pos] = 0;
                }
            }
        }
        printf("%s\n", notes[i].note);
    }
}