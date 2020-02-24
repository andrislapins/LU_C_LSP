/**
 * Author: Engineer Man
 * Code Sorce: https://www.youtube.com/watch?time_continue=609&v=wg8hZxMRwcw&feature=emb_title
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define TABLE_SIZE 10000

// Entry type.
typedef struct entry_t {
    char* key;
    char* value;
    struct entry_t* next;
} entry_t;

// Hash Table type.
typedef struct {
    entry_t** entries;
} ht_t;

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

entry_t* ht_pair(const char* key, const char* value)
{
    // Allocate the entry.
    entry_t* entry = malloc(sizeof(entry) * 1);
    entry->key = malloc(strlen(key) + 1);
    entry->value = malloc(strlen(value) * 1);

    // Copy the key and value in place.
    strcpy(entry->key, key);
    strcpy(entry->value, value);

    // Next starts out NULL but may be se later on.
    entry->next = NULL;

    return entry;
}

ht_t* ht_create(void)
{
    // Allocate table.
    ht_t* hashtable = malloc(sizeof(ht_t) * 1);

    // Allocate table entries.
    hashtable->entries = malloc(sizeof(entry_t*) * TABLE_SIZE);

    // Set each to NULL (needed for proper operation).
    int i = 0;
    for (; i < TABLE_SIZE; ++i) {
        hashtable->entries[i] = NULL;
    }

    return hashtable;
}

void ht_set(ht_t* hashtable, const char* key, const char* value)
{
    unsigned int bucket = hash(key);

    // Try to look up an entry set.
    entry_t* entry = hashtable->entries[bucket];

    // No entry means that bucket is empty. Therefore, insert immediately.
    if (entry == NULL) {
        hashtable->entries[bucket] = ht_pair(key, value);
        return;
    }

    entry_t* prev;

    // Walk through each entry until either the end is reached or a matching
    // key is found.
    while (entry != NULL) {
        // Check key.
        if (strcmp(entry->key, key) == 0) {
            // Match found, replace value.
            free(entry->value);
            entry->value = malloc(strlen(value) * 1);
            strcpy(entry->value, value);
            return;
        }

        // Walk to next.
        prev = entry;
        entry = prev->next;
    }

    // End of chain reached without a match, add new.
    prev->next = ht_pair(key, value);
}

char* ht_get(ht_t* hashtable, const char* key)
{
    unsigned int slot = hash(key);

    // Try to find a valid slot.
    entry_t* entry = hashtable->entries[slot];

    // No slot - means no entry.
    if (entry == NULL) {
        return NULL;
    }

    // Walk through each entry in the slot, which could just be a single thing.
    while (entry != NULL) {
        // Return value if found.
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }

        // Proceed to next key if available.
        entry = entry->next;
    }

    // Reaching here means there were >= 1 entries but no key match.
    return NULL;
}

void ht_dump(ht_t* hashtable)
{
    for (int i = 0; i < TABLE_SIZE; ++i) {
        entry_t* entry = hashtable->entries[i];

        if (entry == NULL) {
            continue;
        }

        printf("slot[%4d]: ", i);

        for(;;) {
            printf("%s=%s ", entry->key, entry->value);

            if (entry->next == NULL) {
                break;
            }

            entry = entry->next;
        }

        printf("\n");
    }
}

int main(int argc, char** argv)
{
    ht_t* ht = ht_create();

    ht_set(ht, "name1", "one");
    ht_set(ht, "name2", "two");
    ht_set(ht, "name3", "three");
    ht_set(ht, "name4", "four");
    ht_set(ht, "name5", "five");
    ht_set(ht, "name6", "six");
    ht_set(ht, "name7", "seven");

    ht_dump(ht);

    return 0;
}