// key_value.cpp

#include "key_value.h"

void key_value::init(char* buf) {
    char tokens[256];
    strcpy(tokens, buf);
    char *valstr = strtok(tokens, ", \n");
    val = atof(valstr);
    strcpy(key, strtok(NULL, ", \n"));
    updated = true;
}

void key_contents::init(char* key) {
    prev_on.clear();
    prev_continue.clear();
    cur_on.clear();
    cur_continue.clear();
    strcpy(prevKey, "");
    strcpy(nextKey, "");
    char buf[8];   
    bool prev_zone = true;
    for (int i = 0; i < strlen(key); i++ ) {
        if ( key[i] == '>' ) {
            prev_zone = false;
            continue;
        }
        if ( key[i] == 'n' || key[i] == 'c') {
            buf[0] = key[i + 1];
            buf[1] = key[i + 2];
            buf[2] = key[i + 3];
            buf[3] = 0;
            if ( prev_zone ) {
                if ( key[i] == 'n' ) {
                    prev_on.push_back(atoi(buf));
                    strcat(prevKey, "n");
                    strcat(prevKey, buf);
                }
                else {
                    prev_continue.push_back(atoi(buf));
                    strcat(prevKey, "c");
                    strcat(prevKey, buf);
                }
            } else {
                if ( key[i] == 'n' ) {
                    cur_on.push_back(atoi(buf));
                    strcat(nextKey, "n");
                    strcat(nextKey, buf);
                }
                else {
                    cur_continue.push_back(atoi(buf));
                    strcat(nextKey, "c");
                    strcat(nextKey, buf);
                }
            }
        }
        if ( key[i] == 't' ) {
            buf[0] = key[i + 1];
            buf[1] = key[i + 2];
            buf[2] = 0;
            strcpy(timing_str, buf);
        }
    }
}

void key_contents::dump() {
    printf("prev on :");
    for (int i = 0; i < prev_on.size(); i++) printf(" %d", prev_on[i]);
    printf("\nprev continue:");
    for (int i = 0; i < prev_continue.size(); i++) printf(" %d", prev_continue[i]);
    printf("\ncur. on :");
    for (int i = 0; i < cur_on.size(); i++) printf(" %d", cur_on[i]);
    printf("\ncur. continue:");
    for (int i = 0; i < cur_continue.size(); i++) printf(" %d", cur_continue[i]);
    printf("\ntiming str: %s\n", timing_str);
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
    //copy(src.begin(), src.end(), back_inserter(dst));
    dst.resize(src.size());
    copy(src.begin(), src.end(), dst.begin());
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
            //if ( items[i].updated ) {
            //    item.updated = items[i].updated;                
            //} else {
            //    item.init_val = items[i].init_val;
            //    item.x_val = items[i].x_val;
            //}
        } else {
            if ( i > 0 ) reduced.push_back(item);
            strcpy(item.key, items[i].key);
            item.val = items[i].val;
            //item.updated = items[i].updated;
            //item.init_val = items[i].init_val;
            //item.x_val = items[i].x_val;
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

/* value for key logical */
float value_for_key_lg(char* key, vector<key_value> &items) {
    int index = items.size();
    int indexMax = index;
    if ( index & 1 ) index += 1;
    int offset = index / 2;
    int dir = -1;
    while ( offset != 0 ) {
        index += dir*offset;
        //printf("%d > ", index);
        int cmp = 0;
        if ( index < 0 ) {
            cmp = -1;
        } else if ( index >= indexMax ) {
            cmp = 1;
        } else {
            cmp = strcmp(items[index].key, key);
        }
        if ( cmp > 0 ) {
            dir = -1;
        }
        else if ( cmp < 0 ) {
            dir = 1;
        }
        else { // cmp == 0:
            return items[index].val;
        }
        if ( (offset != 1) && (offset & 1) ) offset += 1;
        offset /= 2;
        if ( index == 1 && offset == 0 && dir == -1 ) offset = 1;
    }
    return(0.0);
}

float value_for_key_lg_debug(char* key, vector<key_value> &items) {
    int index = items.size();
    int indexMax = index;
    if ( index & 1 ) index += 1;
    int offset = index / 2;
    int dir = -1;
    while ( offset != 0 ) {
        index += dir*offset;
        printf("(%d)%d > ", dir*offset, index);
        int cmp = 0;
        if ( index < 0 ) {
            cmp = -1;
        } else if ( index >= indexMax ) {
            cmp = 1;
        } else {
            cmp = strcmp(items[index].key, key);
        }
        if ( cmp > 0 ) {
            dir = -1;
        }
        else if ( cmp < 0 ) {
            dir = 1;
        }
        else { // cmp == 0:
            return items[index].val;
        }
        if ( (offset != 1) && (offset & 1) ) offset += 1;
        offset /= 2;
        if ( index == 1 && offset == 0 && dir == -1 ) offset = 1;
    }
    return(0.0);
}

/* value for key sequential */
float value_for_key(char* key, vector<key_value> &items) {
    for ( int i = 0; i < items.size(); i++ ) {
        int cmp = strcmp(items[i].key, key);
        if ( cmp > 0 ) {
            return(0.0);
        }
        if (  cmp == 0 ) {
            return items[i].val;
        }
    }
    return(0.0);
}

int index_for_key(char* key, vector<key_value> &items) {
    int i = 0;
    for ( ; i < items.size(); i++ ) {
        int cmp = strcmp(items[i].key, key);
        if ( cmp > 0 ) {
            return i;
        }
        if (  cmp == 0 ) {
            return -i - 1;
        }
    }
    return i;
}

int index_for_key_lg(char* key, vector<key_value> &items) {
    int index = items.size();
    int indexMax = index;
    if ( index & 1 ) index += 1;
    int offset = index / 2;
    int dir = -1;
    while ( offset != 0 ) {
        index += dir*offset;
        //printf("%d > ", index);
        int cmp = 0;
        if ( index < 0 ) {
            cmp = -1;
        } else if ( index >= indexMax ) {
            cmp = 1;
        } else {
            cmp = strcmp(items[index].key, key);
        }
        if ( cmp > 0 ) {
            dir = -1;
        }
        else if ( cmp < 0 ) {
            dir = 1;
        }
        else { // cmp == 0:
            return -index - 1;
        }
        if ( (offset != 1) && (offset & 1) ) offset += 1;
        offset /= 2;
        if ( index == 1 && offset == 0 && dir == -1 ) offset = 1;
    }
    if ( dir > 0 ) index += 1;
    return index;
}

size_t hash3bit(char *key) {
    if ( key == 0 ) return( 0 );
    size_t h = 0;
    while ( *key ) {
        h = ( h << 1 ) ^ ( *key++ );
    }
    return(h%TABLE_LENGTH);
}

void key_value_hash::init(vector<key_value> &items) {
    for ( int i = 0; i < TABLE_LENGTH; i++ ) table[i].clear();
    for ( int i = 0; i < items.size(); i++ ) {
        table[hash3bit(items[i].key)].push_back(items[i]);
    }
}

float key_value_hash::value_for_key_in_hash(char* key) {
    //return value_for_key_lg(key, table[hash3bit(key)]);
    return value_for_key_lg(key, table[hash3bit(key)]);
}

void key_value_hash::dump_table(char* key) {
    int table_index = hash3bit(key);
    printf ("table index:%d items:%d\n", table_index, table[table_index].size());
    float val = value_for_key_lg_debug(key, table[table_index]);
    printf ("... value : %.8f\n", val);
    for ( int i = 0; i < 10; i++ ) {
    //for ( int i = 0; i < table[table_index].size(); i++ ) {
        printf ("key:%s value:%.8f\n", table[table_index][i].key, table[table_index][i].val);
    }
}


int add_map_insert(vector<key_value> &items, vector<key_value> &add) {
    vector<key_value> addnew;
    addnew.clear();
    for (int i = 0; i < add.size(); i++) {
        //int pos = index_for_key(add[i].key, items);
        int pos = index_for_key_lg(add[i].key, items);
        if ( pos < 0 ) {
            pos = -1 - pos;
            items[pos].val += add[i].val;
            items[pos].updated = true;
        } else {
            //vector<key_value>::iterator it = items.begin();
            //items.insert(it + pos, add[i]);
            addnew.push_back(add[i]);
        }
    }
    //map_sort(items);
    items.insert(items.end(), addnew.begin(), addnew.end());
    return items.size();
}

float standard(vector<key_value> &items) {
    float st = 0;
    for ( int i = 0; i < items.size(); i++ ) {
        st += items[i].val*items[i].val;
    }
    return st;
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
        return x_standard_sorted(items_a, items_b)/(sqrtf(standard(items_a)*standard(items_b)));
}

/* functions for correlator */

float calc_standard(vector<key_value> &items, float standard) {
    for ( int i = 0; i < items.size(); i++ ) {
        if ( items[i].updated ) {
            standard -= items[i].init_val*items[i].init_val;
            standard += items[i].val*items[i].val;
        }
    }
    return standard;
}

float x_standard_sorted_snapshot(vector<key_value> &items_a, vector<key_value> &items_b) {
    float xst = 0;
    int ia = 0;
    int ib = 0;
    
    while ( ia < items_a.size() && ib < items_b.size() ) {
        int cmp = strcmp(items_a[ia].key, items_b[ib].key);
        if ( cmp == 0 ) {
            xst += items_a[ia].val * items_b[ib].val;
            items_a[ia].x_val = items_b[ib].val;
            items_b[ib].x_val = items_a[ia].val;
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

float calc_x_standard_sorted(vector<key_value> &items, vector<key_value> &items_ref, float x_standard) {
    for ( int i = 0; i < items.size(); i++ ) {
        if ( items[i].updated ) {
            if ( items[i].x_val < 0 ) {
                items[i].x_val = value_for_key_lg(items[i].key, items_ref);
            }
            x_standard -= items[i].init_val*items[i].x_val;
            x_standard += items[i].val*items[i].x_val;
        }
    }
    return x_standard;
}

float calc_x_standard_sorted_kvh(vector<key_value> &items, key_value_hash &kvh, float x_standard) {
    for ( int i = 0; i < items.size(); i++ ) {
        if ( items[i].updated ) {
            if ( items[i].x_val < 0 ) {
                items[i].x_val = kvh.value_for_key_in_hash(items[i].key);
            }
            x_standard -= items[i].init_val*items[i].x_val;
            x_standard += items[i].val*items[i].x_val;
        }
    }
    return x_standard;
}

void set_update(vector<key_value> &items) {
    for (int i = 0; i < items.size(); i++) {
        items[i].updated = false;
        items[i].init_val = items[i].val;
        items[i].x_val = -1;
    }
}

float correlator::calc_if_a(vector<key_value> &items_a, vector<key_value> &items_b) {
    float calc_standard_a = calc_standard(items_a, standard_a);
    //float calc_standard_a = standard(items_a);
    float calc_x_standard = calc_x_standard_sorted(items_a, items_b, x_standard);
    //float calc_x_standard = x_standard_sorted(items_a, items_b);
    return calc_x_standard/(sqrtf(calc_standard_a*standard_b));
}

float correlator::calc_if_a_kvh(vector<key_value> &items_a, key_value_hash &kvh) {
    float calc_standard_a = calc_standard(items_a, standard_a);
    //float calc_standard_a = standard(items_a);
    float calc_x_standard = calc_x_standard_sorted_kvh(items_a, kvh, x_standard);
    //float calc_x_standard = x_standard_sorted(items_a, items_b);
    return calc_x_standard/(sqrtf(calc_standard_a*standard_b));
}

float correlator::init(vector<key_value> &items_a, vector<key_value> &items_b) {
    standard_a = standard(items_a);
    standard_b = standard(items_b);
    set_update(items_a);
    set_update(items_b);
    x_standard = x_standard_sorted_snapshot(items_a, items_b);
    correlation = x_standard/(sqrtf(standard_a*standard_b));
    return correlation;
}
