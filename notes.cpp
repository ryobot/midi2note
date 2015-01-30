// notes.cpp

#include "notes.h"

void notes::init (char* buf) {
    strcpy(note, buf);
    char tokens[256];
    strcpy(tokens, buf);
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
