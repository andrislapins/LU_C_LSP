/**
 * Author: Andris Lapins, al18011
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <openssl/md5.h>

#define TABLE_SIZE 10000

// Entry type.
typedef struct entry_t {
    char* key;
    char* md5; // In case of calculating MD5 checksum.
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

void printHelpText(void);
int processDirectories(char*);

// These function implementations with certain personal modifications got from
// https://www.youtube.com/watch?time_continue=609&v=wg8hZxMRwcw&feature=emb_title
ht_t* ht_create(void);
entry_t* ht_pair(const char*, const char*);
entry_t* ht_pair_md5_date(const char*, const char*, const char*);
void ht_dump(ht_t*);
unsigned int hash(const char*, long);
void ht_set(ht_t* hashtable, const char*, const char*, long);
unsigned int hash_date(const char*, long, char[80]);
void ht_set_date(ht_t* hashtable, const char*, const char*, long, char[80]);
unsigned int hash_md5(unsigned char[MD5_DIGEST_LENGTH]);
void ht_set_md5(ht_t* hashtable, const char*);
unsigned int hash_md5_date(const char*, long, const char[80], const char[MD5_DIGEST_LENGTH]);
void ht_set_md5_date(ht_t*, const char*, const char*, long, char[80]);

int main(int argc, char** argv)
{
    // Default values.
    md5Flag = 0;
    dateFlag = 0;
    result = 0;

    ht = ht_create();

    // too many flags if stmt

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

    // Output the data stored in hash table.
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
                // Process modification time.
                struct tm lt;
                char date[80];
                time_t t = file_info.st_mtime;
                localtime_r(&t, &lt);
                strftime(date, sizeof(date), "%Y-%m-%d %H:%M", &lt);

                // Check which function to activate.
                if (md5Flag && dateFlag) {
                    ht_set_md5_date(
                        ht, dp->d_name, path, (long)file_info.st_size, date
                    );
                } else if (md5Flag) {
                    ht_set_md5(ht, path);
                } else if (dateFlag) {
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
    printf("md3.o(1)\t\t\tUser Commands\t\t\tmd3.o(1)\n\n");
    printf("NAME\n");
    printf("\tmd3.o - Print out duplicate file paths with equal size ");
    printf("and name\n\n");
    printf("SYNOPSIS\n");
    printf("\t./md3.o [option]...\n\n");
    printf("DESCRIPTION\n");
    printf("\tOutput file duplicate paths according to the passed flags.\n\n");
    printf("\t-h\n\tshows the man page\n\n");
    printf("\t-d\n\tprints out the paths files with equal file size, ");
    printf("name and modification date\n\n");
    printf("\t-m\n\tprints out the paths files with equal md5 checksum\n\n");
    printf("AUTHOR\n");
    printf("\tWritten by Andris Lapins.\n");
}

// hash creates a bucket key from file name and its size.
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

// ht_set creates a new bucket in hash table if bucket is empty. Otherwise,
// function appends to that bucket a new entry. This function is for the case
// of sorting after file name and file size.
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

// hash_date creates a bucket key from file name, file size and file
// modification date.
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

// ht_set_date creates a new bucket in hash table if bucket is empty. Otherwise,
// function appends to that bucket a new entry. This function is for the case
// of sorting after file name, file size and file modification date.
void ht_set_date(
    ht_t* hashtable, 
    const char* key, 
    const char* value, 
    long size, 
    char date[80]
)
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

// hash_md5 creates a bucket key from a string of MD5 checksum.
unsigned int hash_md5(unsigned char key[MD5_DIGEST_LENGTH])
{
    int i;
    unsigned long int value = 0;

    // Do several rounds of multiplication by key characters.
    for (i = 0; i < 16; ++i) {
        value = value * 37 + key[i];
    }

    // Make sure value is 0 <= value < TABLE_SIZE.
    value = value % TABLE_SIZE;

    return value;
}

// ht_set_md5 creates a new bucket in hash table if bucket is empty. Otherwise,
// function appends to that bucket a new entry. This function is for the case
// of sorting after file's MD5 checksum.
void ht_set_md5(ht_t* hashtable, const char* path)
{
    // MD5 logic implementation was taken from
    // https://stackoverflow.com/questions/10324611/how-to-calculate-the-md5-hash-of-a-large-file-in-c
    FILE *inFile = fopen(path, "rb");
    if (inFile == NULL) {
        printf("%s can not be opened\n", path);
        return;
    }

    unsigned char c[MD5_DIGEST_LENGTH];
    MD5_CTX mdContext;
    int bytes;
    unsigned char data[1024];
    MD5_Init(&mdContext);

    while ((bytes = fread(data, 1, 1024, inFile)) != 0) {
        MD5_Update(&mdContext, data, bytes);
    }

    MD5_Final(c, &mdContext);
    fclose(inFile);

    // Assigning the md5 hash as key.
    unsigned int bucket = hash_md5(c);

    // Try to look up an entry set.
    entry_t* entry = hashtable->entries[bucket];

    // No entry means that bucket is empty. Therefore, insert immediately.
    if (entry == NULL) {
        hashtable->entries[bucket] = ht_pair(c, path);
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
    prev->next = ht_pair(c, path);
}

// hash_md5_date creates a bucket key from a string of MD5 checksum.
unsigned int hash_md5_date(
    const char* key, long size, const char date[80], const char md5[MD5_DIGEST_LENGTH]
)
{
    int i;
    unsigned long int value = 0;
    unsigned int key_len = strlen(key);
    unsigned int date_len = strlen(date);

    // Do several rounds of multiplication by md5 characters.
    for (i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        value = value * 37 + md5[i];
    }

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

// ht_set_md5_date creates a new bucket in hash table if bucket is empty. Otherwise,
// function appends to that bucket a new entry. This function is for the case
// of sorting after file's MD5 checksum.
void ht_set_md5_date(
    ht_t* hashtable, 
    const char* key, 
    const char* path, 
    long size, 
    char date[80]
)
{
    // MD5 logic implementation was taken from
    // https://stackoverflow.com/questions/10324611/how-to-calculate-the-md5-hash-of-a-large-file-in-c
    FILE *inFile = fopen(path, "rb");
    if (inFile == NULL) {
        printf("%s can not be opened\n", path);
        return;
    }

    unsigned char c[MD5_DIGEST_LENGTH];
    MD5_CTX mdContext;
    int bytes;
    unsigned char data[1024];
    MD5_Init(&mdContext);

    while ((bytes = fread(data, 1, 1024, inFile)) != 0) {
        MD5_Update(&mdContext, data, bytes);
    }

    MD5_Final(c, &mdContext);
    fclose(inFile);

    // Assigning the md5 hash as key.
    unsigned int bucket = hash_md5_date(key, size, date, c);

    // Try to look up an entry set.
    entry_t* entry = hashtable->entries[bucket];

    // No entry means that bucket is empty. Therefore, insert immediately.
    if (entry == NULL) {
        hashtable->entries[bucket] = ht_pair_md5_date(key, path, c);
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
    prev->next = ht_pair(key, path);
}

// ht_pair allocates memory for the entry and its fields and return it.
entry_t* ht_pair(const char* key, const char* value)
{
    struct stat file_info; 

    // Check if getting file attributes is possible.
    if (lstat(value, &file_info) < 0) {
        printf("Could not get file attributes for pairing.\n");
    }

    // Allocate the entry and main hash fields.
    entry_t* entry = malloc(sizeof(entry_t) * 1);
    entry->key = malloc(strlen(key) + 1);
    entry->value = malloc(strlen(value) * 1);

    struct tm lt;
    time_t t = file_info.st_mtime;
    char timbuf[80];

    // Get the string of date and allocate memory for it.
    localtime_r(&t, &lt);
    strftime(timbuf, sizeof(timbuf), "%Y-%m-%d %H:%M", &lt);
    entry->date = malloc(sizeof(timbuf));

    // Assign the values to the entry
    strcpy(entry->key, key);
    strcpy(entry->value, value);
    entry->size = file_info.st_size;
    strcpy(entry->date, timbuf);
    entry->next = NULL;

    return entry;
}

// ht_pair allocates memory for the entry and its fields and return it.
entry_t* ht_pair_md5_date(const char* key, const char* value, const char md5[16])
{
    struct stat file_info; 

    // Check if getting file attributes is possible.
    if (lstat(value, &file_info) < 0) {
        printf("Could not get file attributes for pairing.\n");
    }

    // Allocate the entry and main hash fields.
    entry_t* entry = malloc(sizeof(entry_t) * 1);
    entry->key = malloc(strlen(key) + 1);
    entry->value = malloc(strlen(value) * 1);
    entry->md5 = malloc(strlen(md5));

    struct tm lt;
    time_t t = file_info.st_mtime;
    char timbuf[80];

    // Get the string of date and allocate memory for it.
    localtime_r(&t, &lt);
    strftime(timbuf, sizeof(timbuf), "%Y-%m-%d %H:%M", &lt);
    entry->date = malloc(sizeof(timbuf));

    // Assign the values to the entry
    strcpy(entry->key, key);
    strcpy(entry->value, value);
    strcpy(entry->md5, md5);
    entry->size = file_info.st_size;
    strcpy(entry->date, timbuf);
    entry->next = NULL;

    return entry;
}

// ht_create creates and returns a new hash table.
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
// configured by command flags.
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

        if (md5Flag && dateFlag) {
            printf("=== %s %ld %s", entry->date, entry->size, entry->key);
            for (int i = 0;i < 16; i++) {
                printf("%0x", (unsigned char)entry->md5[i]);
            }
            printf("\n");
        }
        else if (md5Flag) {
            printf("=== %s %ld ", entry->date, entry->size);
            for (int i = 0;i < 16; i++) {
                printf("%0x", (unsigned char)entry->key[i]);
            }
            printf("\n");
        } else {
            printf("=== %s %ld %s\n", entry->date, entry->size, entry->key);
        }
         

        while(entry != NULL) {
            printf("%s\n", entry->value);

            entry = entry->next;
        }

        printf("\n");
    }
}