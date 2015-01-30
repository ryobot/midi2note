// key_value.cpp

#include "key_value.h"

void key_value::init(char* buf) {
    char tokens[256];
    strcpy(tokens, buf);
    char *valstr = strtok(tokens, ", \n");
    val = atof(valstr);
    strcpy(key, strtok(NULL, ", \n"));
}

int load_key_value_file(char* filename, vector<key_value> &items, bool reduced) {
    FILE *f;
    char buf[256];
    key_value item;

    if ( NULL == ( f = fopen(filename, "r"))) {
        printf("file %s not found\n", filename);
        exit(1);
    }
    while( fgets(buf, 255, f) != NULL ) {
        item.init(buf);
        if ( !reduced ) {
            bool found = false;
            for ( int i = 0; i < items.size(); i++ ) {
                if ( strcmp(items[i].key, item.key) == 0 ) {
                    items[i].val += item.val;
                    found = true;
                    break;
                }
            }
            if ( found ) continue;
        }
        items.push_back(item);
    }
    fclose(f);
    return(0);
}

float value_for_key(char* key, vector<key_value> &items) {
    for ( int i = 0; i < items.size(); i++ ) {
        if ( strcmp(items[i].key, key) == 0 ) {
            return items[i].val;
        }
    }
    return(0.0);
}

float standard(vector<key_value> &items) {
    float st = 0;
    for ( int i = 0; i < items.size(); i++ ) {
        st += items[i].val*items[i].val;
    }
    return sqrtf(st);
}

float x_standard(vector<key_value> &items_a, vector<key_value> &items_b) {
    float xst = 0;
    for ( int i = 0; i < items_a.size(); i++ ) {
        xst += items_a[i].val*value_for_key(items_a[i].key, items_b);
    }
    return xst;
}

float correlation(vector<key_value> &items_a, vector<key_value> &items_b) {
        return x_standard(items_a, items_b)/(standard(items_a)*standard(items_b));
}
