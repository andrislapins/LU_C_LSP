/**
 * Author: Engineer Man
 * Code Sorce: https://www.youtube.com/watch?time_continue=609&v=wg8hZxMRwcw&feature=emb_title
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define TABLE_SIZE 10000

typedef struct entry_t {
    char* key;
    char* value;
    struct entry_t* next;
} entry_t;

typedef struct {
    entry_t** entries;
} hash_table_t;



unsigned int hash(const char *key)
{
    unsigned long int value = 0;
    unsigned int i = 0;
    unsigned int key_len = strlen(key);

    // Do several rounds of multiplication.
    for (; i < key_len; ++i) {
        value = value * 37 + key[i];
    }

    // Make sure value is 0 <= value < TABLE_SIZE.
    value = value % TABLE_SIZE;

    return value;
}

int main(int argc, char** argv)
{
    printf("%d\n", hash("em"));

    return 0;
}