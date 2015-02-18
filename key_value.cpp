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

int mapcopy(vector<key_value> &dst, vector<key_value> &src) {
    dst.clear();
    copy(src.begin(), src.end(), back_inserter(dst));
    return dst.size();
}

bool operator>( const key_value& left, const key_value& right ) {
    //return left.val > right.val;
    return strcmp(left.key, right.key) > 0;
}

bool operator<( const key_value& left, const key_value& right ) {
    //return left.val < right.val;
    return strcmp(left.key, right.key) < 0;
}

void map_sort(vector<key_value> &items) {
    sort(items.begin(), items.end());
}

int map2reduce(vector<key_value> &items) {
    vector<key_value> reduced;
    map_sort(items);
    key_value item;
    strcpy(item.key, "");
    item.val = 0.0;
    for ( int i = 0; i < items.size(); i++ ) {
        if ( strcmp(item.key, items[i].key) == 0 ) {
            item.val += items[i].val;
        } else {
            if ( i > 0 ) reduced.push_back(item);
            strcpy(item.key, items[i].key);
            item.val = items[i].val;
        }
    }
    reduced.push_back(item);
    mapcopy(items, reduced);
    return items.size();
}

int add_map_old(vector<key_value> &items, vector<key_value> &add) {
    for ( int i = 0; i < add.size(); i++ ) {
        bool found = false;
        for ( int j = 0; j < items.size(); j++ ) {
            if ( strcmp(add[i].key, items[j].key) == 0 ) {
                items[j].val += add[i].val;
                found = true;
                break;
            }
        }
        if ( found ) continue;
        items.push_back(add[i]);
    }
    return items.size();
}

int add_map(vector<key_value> &items, vector<key_value> &add) {
    items.insert(items.end(), add.begin(), add.end());
    map2reduce(items);
    return items.size();
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

float x_standard_sorted(vector<key_value> &items_a, vector<key_value> &items_b) {
    float xst = 0;
    int ia = 0;
    int ib = 0;
    
    while ( ia < items_a.size() && ib < items_b.size() ) {
        int cmp = strcmp(items_a[ia].key, items_b[ib].key);
        if ( cmp == 0 ) {
            xst += items_a[ia].val * items_b[ib].val;
            ia++;
            ib++;
        } else if ( cmp < 0 ) {
            ia++;
        } else {
            ib++;
        }
    }
    return xst;
}

float correlation(vector<key_value> &items_a, vector<key_value> &items_b) {
        //return x_standard(items_a, items_b)/(standard(items_a)*standard(items_b));
        return x_standard_sorted(items_a, items_b)/(standard(items_a)*standard(items_b));
}
