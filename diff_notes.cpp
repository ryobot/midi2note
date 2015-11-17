// unittest.cpp
//#include <typeinfo>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <string.h>
using namespace std;

#include "note_map.h"

vector<notes> notes1;
vector<notes> notes2;

int main(int argc, char *argv[])
{
    // initializations:
    //setvbuf(stdout, (char *)NULL, _IONBF, 0);
    //init_note_num();
    
    if ( argc < 2 ) {
        printf("usage : diff_notes [txt note file1] [txt note file2]\n");
        exit(1);
    }

    notes1.clear();
    load_note_file(argv[1], notes1);
    notes2.clear();
    load_note_file(argv[2], notes2);
    int len = notes1.size();
    if ( len > notes2.size() )
        len = notes2.size();

    for ( int i = 0; i < len; i++ ) {
        for (int c = 0; c < strlen(notes1[i].note); c++) {
            if (c >= MIN_NOTE_POS &&  notes1[i].note[c] != notes2[i].note[c] ) {
                if ( is_note_ch(notes1[i].note[c]) ) {
                    printf("\e[33m");
                    printf("%c", notes1[i].note[c]);
                    printf("\e[m");
                } else {
                    printf("\e[34m");
                    printf("%c", notes2[i].note[c]);
                    printf("\e[m");
                }
            } else {
                printf("%c", notes1[i].note[c]);
            }
        }
        printf("\n");
    }
    
    exit(0);
}