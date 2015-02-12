// notes.cpp

#include "notes.h"

void notes::init (char* buf) {
    char tokens[256];
    strcpy(tokens, buf);
    strcpy(note, strtok(tokens, "\n"));
    char *valstr = strtok(tokens, ", \n");
    time = atoi(valstr);
}

int load_note_file(char* filename, vector<notes> &items) {
    FILE *f;
    char buf[256];
    notes item;

    if ( NULL == ( f = fopen(filename, "r"))) {
        printf("file %s not found\n", filename);
        exit(1);
    }
    while( fgets(buf, 255, f) != NULL ) {
        if ( strlen(buf) > 16 ) {
            item.init(buf);
            items.push_back(item);
        }
    }
    fclose(f);
    return(items.size());
}

int load_mask_file(char* filename, char *mask) {
    FILE *f;
    char buf[256];

    if ( NULL == ( f = fopen(filename, "r"))) {
        printf("file %s not found\n", filename);
        exit(1);
    }
    while( fgets(buf, 255, f) != NULL ) {
        if ( strlen(buf) > 16 ) {
            strcpy(mask, strtok(buf, "\n"));
        }
    }
    fclose(f);
    return(strlen(mask));
}

char* get_last_notes(char* buf, vector<notes> &items) {
    strcpy(buf, items[items.size() - 1].note);
    return buf;
}