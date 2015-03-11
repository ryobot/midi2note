// midi2note.cpp
#include <typeinfo>
#include <cstdlib>
#include <string>
#include <stdio.h>
#include <string.h>
using namespace std;

int getNote(string line) {
    int from = line.find("n=", 0);
    int to = line.find(" ", from);
    string noteStr = line.substr(from + 2, to - from - 2);
    return(std::atoi(noteStr.c_str()));
}

int getVelocity(string line) {
    int from = line.find("v=", 0);
    int to = line.find(" ", from);
    string velStr = line.substr(from + 2, to - from - 2);
    return(std::atoi(velStr.c_str()));
}

int getTime(string line) {
    int from = 0;
    int to = line.find(" ", from);
    string timeStr = line.substr(from, to - from);
    return(std::atoi(timeStr.c_str()));
}

int getDataKind(string line) {
    if ( line.find("On", 0) != string::npos ) {
        if ( 0 < getVelocity(line) ) return(1);
        else return(2);
    }
    if ( line.find("Off", 0) != string::npos ) return(2);
    return(0);
}

int main(int argc, char *argv[])
{
    if ( argc < 2 ) {
        printf("usage : midi2note [txt midi file of mf2t]\n", argv[1]);
        exit(1);
    }
    FILE *f;
    if ( NULL == ( f = fopen(argv[1], "r"))) {
        printf("file %s not found\n", argv[1]);
        exit(1);
    }
    int transport = 0;
    if ( argc == 3 ) {
        transport = atoi(argv[2]);
    }
    
    char buf[256];
    char n[128];
    strcpy(n, "|           |           |           |           |           |           |");
    int t = 0;
    int time = 0;
    int delta = 240;
    while( fgets(buf, 255, f) != NULL ) {
        string line(buf);
        int note = 0;
        int dataKind = getDataKind(line);
        if ( dataKind > 0 ) {
            note = getNote(line);
            time = getTime(line);
            while ( t < time ) {
                if ( t % 960 == 0 ) {
                    char nx[128];
                    strcpy(nx, n);
                    for ( int i = 0; i < 72; i++) {
                        if ( nx[i] == ' ' ) nx[i] = '-';
                    }
                    printf("\n%06d : %s", t, nx);
                } else {
                    printf("\n%06d : %s", t, n);
                }
                t += delta;
                for ( int i = 0; i < 72; i++) {
                    if ( n[i] == 'o' ) n[i] = '+';
                }
            }
            int disp_pos = note - 24 + transport;
            if ( disp_pos >= 0 && disp_pos < 73 ) {
                if ( dataKind == 2 ) {
                    if ( disp_pos % 12 == 0 ) {
                         n[disp_pos] = '|';
                    } else {
                         n[disp_pos] = ' ';
                    }
                }
                if ( dataKind == 1 ) n[disp_pos] = 'o';
            }
        }
    }
    printf("\n");
    fclose(f);
}