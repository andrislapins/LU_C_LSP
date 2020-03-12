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
    char* date;
    long size;
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
void ht_dump(ht_t*);

unsigned int hash(const char*, long);
void ht_set(ht_t* hashtable, const char*, const char*, long);

unsigned int hash_date(const char*, long, char[80]);
void ht_set_date(ht_t* hashtable, const char*, const char*, long, char[80]);

unsigned int hash_md5(const char*, long, char[80]);
void ht_set_md5(ht_t* hashtable, const char*, const char*, long, char[80]);

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

    // Outputting the data of the hash table.
    ht_dump(ht);

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
                if (md5Flag) {
                    // ht_set_md5(ht, dp->d_name, path);
                } else if (dateFlag) {
                    struct tm lt;
                    char date[80];
                    time_t t = file_info.st_mtime;
                    localtime_r(&t, &lt);
                    strftime(date, sizeof(date), "%Y-%m-%d %H:%M", &lt);
                    ht_set_date(
                        ht, dp->d_name, path, (long)file_info.st_size, date
                    );
                } else { 
                    ht_set(ht, dp->d_name, path, (long)file_info.st_size);
                }
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

// hash...
unsigned int hash(const char *key, long size)
{
    unsigned long int value = 0;
    unsigned int i = 0;
    unsigned int key_len = strlen(key);

    // Do several rounds of multiplication.
    for (; i < key_len; ++i) {
        value = value * 37 + key[i] + size;
    }

    // Make sure value is 0 <= value < TABLE_SIZE.
    value = value % TABLE_SIZE;

    return value;
}
// ht_set ... 
void ht_set(ht_t* hashtable, const char* key, const char* value, long size)
{
    unsigned int bucket = hash(key, size);

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

// ...
unsigned int hash_date(const char *key, long size, char date[80])
{
    int i;
    unsigned long int value = 0;
    unsigned int key_len = strlen(key);
    unsigned int date_len = strlen(date);

    // Do several rounds of multiplication by key characters.
    for (i = 0; i < key_len; ++i) {
        value = value * 37 + key[i] + size;
    }

    // Do several rounds of multiplication by date characters.
    for (i = 0; i < date_len; ++i) {
        value = value * 37 + date[i] + size;
    }

    // Make sure value is 0 <= value < TABLE_SIZE.
    value = value % TABLE_SIZE;

    return value;
}

// ht_set ... 
void ht_set_date(ht_t* hashtable, const char* key, const char* value, long size, char date[80])
{
    unsigned int bucket = hash_date(key, size, date);

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

// hash_md5
unsigned int hash_md5(const char *key, long size, char date[80])
{
    int i;
    unsigned long int value = 0;
    unsigned int key_len = strlen(key);
    unsigned int date_len = strlen(date);

    // Do several rounds of multiplication by key characters.
    for (i = 0; i < key_len; ++i) {
        value = value * 37 + key[i] + size;
    }

    // Do several rounds of multiplication by date characters.
    for (i = 0; i < date_len; ++i) {
        value = value * 37 + date[i] + size;
    }

    // Make sure value is 0 <= value < TABLE_SIZE.
    value = value % TABLE_SIZE;

    return value;
}

// ht_set_md5 ... 
void ht_set_md5(ht_t* hashtable, const char* key, const char* value, long size, char date[80])
{
    unsigned int bucket = hash_date(key, size, date);

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

// ht_pair...
entry_t* ht_pair(const char* key, const char* value)
{
    // Allocate the entry and main hash fields.
    entry_t* entry = malloc(sizeof(entry_t) * 1);
    entry->key = malloc(strlen(key) + 1);
    entry->value = malloc(strlen(value) * 1);

    // Copy the key and value in place.
    strcpy(entry->key, key);
    strcpy(entry->value, value);

    // Set the next entry in the same bucket as NULL.
    entry->next = NULL;

    return entry;
}

// ht_create
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

// ht_dump prints duplicates of files which are equal by certain properties
// configured by launching the program.
void ht_dump(ht_t* hashtable)
{
    for (int i = 0; i < TABLE_SIZE; ++i) {
        entry_t* entry = hashtable->entries[i];

        // In case bucket is empty.
        if (entry == NULL) {
            continue;
        }

        // In case a bucket does not have multiple entries.
        if (entry->next == NULL) {
            continue;
        }

        printf("=== %s %ld %s\n", entry->date, entry->size, entry->key);

        while(entry != NULL) {
            printf("%s\n", entry->value);

            entry = entry->next;
        }

        printf("\n");
    }
}