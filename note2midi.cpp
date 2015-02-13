// note2midi.cpp
//#include <typeinfo>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <string.h>
using namespace std;

#include "note_map.h"

vector<key_value> map;
vector<notes> notes;

struct midi {
    int n;
    int ch;
    int v;
};

vector<midi> running_notes;

void print_header() {
    printf("MFile 0 1 960\n");
    printf("MTrk\n");
    printf("0 Meta SeqName \"midi2note sequence\"\n");
    printf("0 TimeSig 1/4 24 8\n");
    printf("0 Tempo 250000\n");
    printf("0 SMPTE 96 0 0 0 0\n");
    printf("0 SysEx f0 7e 7f 09 01 f7\n");
    printf("0 SysEx f0 7f 7f 04 01 00 7f f7\n");
    printf("200 Par ch=1 c=0 v=0\n");
    printf("202 Par ch=1 c=32 v=0\n");
    printf("204 PrCh ch=1 p=0\n");
    printf("206 Par ch=1 c=7 v=110\n");
    printf("208 Par ch=1 c=10 v=64\n");
    printf("960 TimeSig 4/4 24 8\n");
    printf("960 Tempo 1600000\n");
}

void print_footer(int time) {
    printf("%d Par ch=1 c=64 v=0\n", time);
    printf("%d Meta TrkEnd\n", time);
    printf("TrkEnd\n");
}

int main(int argc, char *argv[])
{
    if ( argc < 2 ) {
        printf("usage : note2midi [txt note file]\n");
        exit(1);
    }
    print_header();
    load_note_file(argv[1], notes);
    int time = 0;
    for (int i = 0; i < notes.size(); i++ ) {
        time = notes[i].time;
        for (int j = MIN_NOTE_POS; j < MAX_NOTE_POS; j++ ) {
            int note_num = j + NOTE_NUM_OFFSET - MIN_NOTE_POS;
            if ( notes[i].note[j] != '+' ) {
                for ( int k = 0; k < running_notes.size(); k++ ) {
                    if ( running_notes[k].n == note_num ) {
                        printf("%d Off ch=%d n=%d v=%d\n", notes[i].time, running_notes[k].ch, running_notes[k].n, running_notes[k].v);
                        running_notes.erase(running_notes.begin() + k);
                        break;
                    }
                } 
            }
            if ( notes[i].note[j] == 'o' ) {
                midi md;
                md.n = note_num;
                md.ch = 1;
                md.v = 64;
                running_notes.push_back(md);
                printf("%d On ch=%d n=%d v=%d\n", notes[i].time, md.ch, md.n, md.v);
            }
        }
    }
    time += 240;
    for ( int k = 0; k < running_notes.size(); k++ ) {
        printf("%d Off ch=%d n=%d v=%d\n", time, running_notes[k].ch, running_notes[k].n, running_notes[k].v);
    } 
    time += 240;
    print_footer(time);
}