/**
 * Author: Andris Lapins, al18011
 */

// TODO:
// Implement the cut array func as helper func later
// Optimise everyting later:
// having struct stat in ht_get. pass a pointer better

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>

#define TABLE_SIZE 10000

// Entry type.
typedef struct entry_t {
    char* key;
    char* value;
    long size;
    char* date;
    struct entry_t* next;
} entry_t;

// Hash Table type.
typedef struct {
    entry_t** entries;
} ht_t;

ht_t* ht; // Hash table to store all the occurrences of every file.
int md5Flag; // Boolean to check if file comparison by MD5 hash is needed.
int dateFlag; // Boolean to check if file comparison by date is needed.
int result; // Stores the returned value to main function from other functions.

// My personal functions.
void printHelpText(void);
int processDirectories(char*);

// The function implementations with certain personal modifications got from
// https://www.youtube.com/watch?time_continue=609&v=wg8hZxMRwcw&feature=emb_title
ht_t* ht_create(void);
entry_t* ht_pair(const char*, const char*);
void ht_set(ht_t* hashtable, const char*, const char*);
unsigned int hash(const char*);
char* ht_get(ht_t*, const char*);
void ht_dump(ht_t*);
void ht_dump_date(ht_t*);
void ht_dump_md5(ht_t*);

int main(int argc, char** argv)
{
    // Default values.
    md5Flag = 0;
    dateFlag = 0;
    result = 0;

    ht = ht_create();

    // Checking the flags passed to the command of this program.
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            printHelpText();
            return 0;
        } else if (strcmp(argv[i], "-m") == 0) {
            md5Flag = 1;
        } else if (strcmp(argv[i], "-d") == 0) {
            dateFlag = 1;
        } else {
            printf("Incorrect format of the command and its flags\n");
            return 1;
        }
    }

    // Run the program starting from the current directory.
    result = processDirectories(".");

    // Printing according by the given flags to the command.
    if (md5Flag) {
        ht_dump_md5(ht);
    } else if (dateFlag) {
        ht_dump_date(ht);
    } else {
        ht_dump(ht);
    }

    printf("\n");
    return result;
}

// processDirectories goes through the directories of the passed in
// directory path.
int processDirectories(char* basePath)
{
    DIR* dir;
    struct dirent* dp;
    struct stat file_info;
    char path[1000];

    // Open the current directory of the launched command.
    dir = opendir(basePath);
    if (dir == NULL) {
        printf("An error occurred opening the directory %s\n", basePath);
        return 1;
    }

    // Read the next file until no file detected.
    while((dp = readdir(dir)) != NULL) {
        // Ignore cases when . or .. detected.
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
            // Construct new path from the basePath.
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);

            // Check if getting file attributes is possible.
            if (lstat(path, &file_info) < 0) {
                printf("Could not get file attributes.\n");
                return 1;
            }

            // Checking if the current file is directory.
            if (file_info.st_mode&__S_IFDIR) {
                processDirectories(path);
            } else {
                // Adding the file name as key and path to it as an occurrence.
                ht_set(ht, dp->d_name, path);
            }
        }
    }

    // Close directory stream.
    closedir(dir);
    return 0;
}

// printHelpText prints the manual of using the command of this program.
void printHelpText()
{
    printf("Help text\n");
}

// --- Hash functions --- 

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
    struct stat file_info;

    // Check if getting file attributes is possible.
    if (lstat(value, &file_info) < 0) {
        printf("Could not get file attributes for pairing.\n");
    }

    // Allocate the entry.
    entry_t* entry = malloc(sizeof(entry_t) * 1);
    entry->key = malloc(strlen(key) + 1);
    entry->value = malloc(strlen(value) * 1);

    // Get the string of date and allocate memory for it.
    time_t t = file_info.st_mtime;
    struct tm lt;
    localtime_r(&t, &lt);
    char timbuf[80];
    strftime(timbuf, sizeof(timbuf), "%Y-%m-%M", &lt);
    // char* date = ctime(&file_info.st_mtime);
    entry->date = malloc(sizeof(timbuf));

    // Copy the key and value in place.
    strcpy(entry->key, key);
    strcpy(entry->value, value);
    // Store meta data.
    strcpy(entry->date, timbuf);
    entry->size = file_info.st_size;

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

    // Walk through each entry until the end is reached.
    while (entry != NULL) {
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

// Print duplicates of files which are equal by name and size.
void ht_dump(ht_t* hashtable)
{
    printf("ht_dump\n");

    // struct stat file_info;
    
    for (int i = 0; i < TABLE_SIZE; ++i) {
        entry_t* entry = hashtable->entries[i];

        if (entry == NULL) {
            continue;
        }

        printf("=== %s %ld %s\n", entry->date, entry->size, entry->key);

        for(;;) {
            printf("%s\n", entry->value);

            if (entry->next == NULL) {
                break;
            }

            entry = entry->next;
        }

        printf("\n");
    }
}

//
void ht_dump_date(ht_t* hashtable)
{
    printf("ht_dump_date\n");
    for (int i = 0; i < TABLE_SIZE; ++i) {
        entry_t* entry = hashtable->entries[i];

        if (entry == NULL) {
            continue;
        }

        printf("slot[%4d]: ", i);

        for(;;) {
            printf("(%s=%s) ", entry->key, entry->value);

            if (entry->next == NULL) {
                break;
            }

            entry = entry->next;
        }

        printf("\n");
    }
}

//
void ht_dump_md5(ht_t* hashtable)
{
    printf("ht_dump_md5\n");
    for (int i = 0; i < TABLE_SIZE; ++i) {
        entry_t* entry = hashtable->entries[i];

        if (entry == NULL) {
            continue;
        }

        printf("slot[%4d]: ", i);

        for(;;) {
            printf("(%s=%s) ", entry->key, entry->value);

            if (entry->next == NULL) {
                break;
            }

            entry = entry->next;
        }

        printf("\n");
    }
}