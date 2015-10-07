// unittest.cpp
//#include <typeinfo>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <string.h>
using namespace std;

#include "key_value.h"

vector<key_value> items;

int main(int argc, char *argv[])
{
    if ( argc < 2 ) {
        printf("usage : unittest [txt map file]\n");
        exit(1);
    }
    
    int argc_pos = 1;
    bool reduced = true;
    
    // load map:
    load_key_value_file(argv[argc_pos++], items, reduced);
    printf ("item : %d data\n", items.size());

    char key[128];
    
    /* hash test */
    /*
    key_value_hash kvh;
    kvh.init(items);
    for ( int i = 0; i < items.size(); i++ ) {
        strcpy(key, items[i].key);
        if ( kvh.value_for_key_in_hash(key) != value_for_key_lg(key, items) ) {
            printf ("key : %s\n", key);
            printf ("value for key (lg) : %.8f\n", value_for_key_lg(key, items));
            printf ("value for key (hash) : %.8f\n", kvh.value_for_key_in_hash(key));
            kvh.dump_table(key);
        }
    }*/

    vector<key_value> test_items;
    for ( int len = 10; len < 100; len++ ) {
        printf ("length : %d\n", len);
        test_items.clear();
        for (int i = 0; i < len; i++) {
            test_items.push_back(items[i]);
        }
        for ( int i = 0; i < len; i++ ) {
            strcpy(key, test_items[i].key);
            bool error = false;
            if ( index_for_key(key, test_items) != index_for_key_lg(key, test_items) ) {
                printf("\e[31m");
                error = true;
            }
            if (error) {
                printf ("  key : %s\n", key);
                printf ("  index for key (lg) : %d\n", index_for_key_lg(key, test_items));
                printf ("  index for key (seq) : %d\n", index_for_key(key, test_items));
            }
            if ( error ) printf("\e[m");
        }
    }
    
    /*
    int cnt[TABLE_LENGTH];
    for ( int i = 0; i < TABLE_LENGTH; i++ ) {
        cnt[i] = 0;
    }
    for ( int i = 0; i < items.size(); i++) {
        cnt[hash3bit(items[i].key)%TABLE_LENGTH]++;
    }
    int min = items.size();
    int max = 0;
    for ( int i = 0; i < TABLE_LENGTH; i++ ) {
        if ( cnt[i] > max ) max = cnt[i];
        if ( cnt[i] < min ) min = cnt[i];
        printf("table:%03d:%d\n", i, cnt[i]);
    }
    printf("min:%d / max:%d\n", min, max);
    */
    
    /*    
    printf("key?:");
    scanf("%s", key);
    printf ("index for key : %d\n", index_for_key(key, items));
    printf ("index for key (lg) : %d\n", index_for_key_lg(key, items));
    printf ("value for key : %.8f\n", value_for_key(key, items));
    printf ("value for key (lg) : %.8f\n", value_for_key_lg(key, items));
    */
        
    /*
    for ( int i = 0; i < items.size(); i++ ) {
        strcpy(key, items[i].key);
        //if ( index_for_key(key, items) != index_for_key_lg(key, items) ) {
        //    printf ("index for key : %d\n", index_for_key(key, items));
        //    printf ("index for key (lg) : %d\n", index_for_key_lg(key, items));
        //}
        if ( value_for_key(key, items) != value_for_key_lg(key, items) ) {
            printf ("value for key : %.8f\n", value_for_key(key, items));
            printf ("value for key (lg) : %.8f\n", value_for_key_lg(key, items));
        }
        //strcat(key, "o");
        //if ( index_for_key(key, items) != index_for_key_lg(key, items) ) {
        //    printf ("index for key : %d\n", index_for_key(key, items));
        //    printf ("index for key (lg) : %d\n", index_for_key_lg(key, items));
        //}
        //if ( value_for_key(key, items) != value_for_key_lg(key, items) ) {
        //    printf ("value for key : %.8f\n", value_for_key(key, items));
        //    printf ("value for key (lg) : %.8f\n", value_for_key_lg(key, items));
        //}
    }*/
    exit(0);
}