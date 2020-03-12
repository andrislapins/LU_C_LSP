/**
 * Author: Andris Lapins, al18011
 * Description: I wrote this program to compare efficiencies of different
 * dynamic memory allocation functions.
 * NOTE: Please test the functions seperately.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdint.h>

#define MAP_ANONYMOUS 0x20 // [why was this undefined?]

// max_malloc tests how many MB can malloc provide.
void max_malloc()
{
    // Stores value of one megabyte.
    size_t one_mb = 1024*1024;
    // Pointer to the allocated memory address.
    void* p;

    for (size_t i = 1;; i++) {
        p = (void*)malloc(one_mb*i);
        if (p == NULL) {
            perror("malloc failed to allocate more memory");
            exit(EXIT_FAILURE);
        }
        
        free(p);

        printf("%ldMB\n", i);
    }

    exit(EXIT_SUCCESS);
    // NOTE: Use mallopt() to adjust MMAP_THRESHOLD?
}

// MB_100_malloc is for testing how fast can malloc provide 100MB of memory.
// Test this function with program (time).
void MB_100_malloc()
{
    // Stores value of one hundred megabyte.
    size_t one_hundred_mb = 1024*1024*100;

    void* p = (void*)malloc(one_hundred_mb);
    if (p == NULL) {
        perror("malloc failed to allocate 100MB of memory");
        exit(EXIT_FAILURE);
    }

    free(p);

    exit(EXIT_SUCCESS);
}

// max_mmap tests how many MB can mmap provide.
void max_mmap()
{
    // Stores value of one megabyte.
    size_t one_mb = 1024*1024;
    // Result of munmap().
    int r;
    // Pointer to the allocated memory address.
    void* p;

    for (size_t i = 1;; i++) {
        p = mmap(
            NULL,
            one_mb*i,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS,
            -1,
            0
        );
        if (p == MAP_FAILED) {
            perror("mmap failed to allocate more memory");
            exit(EXIT_FAILURE);
        }

        r = munmap(p, one_mb*i);
        if (r == -1) {
            perror("munmap failed to deallocate memory");
            exit(EXIT_FAILURE);
        }

        printf("%ldMB\n", i);
    }

    exit(EXIT_SUCCESS);
}

// MB_100_mmap is for testing how fast can mmap provide 100MB of memory.
// Test this function with program (time).
void MB_100_mmap()
{
    // Stores value of one hundred megabyte.
    size_t one_hundred_mb = 1024*1024*100;
    // Result of munmap().
    int r;
    // Pointer to the allocated memory address.
    void* p;

    p = mmap(
        NULL, 
        one_hundred_mb, 
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS, 
        -1, 
        0
    );
    if (p == MAP_FAILED) {
        perror("sbrk failed to allocate 100MB of memory");
        exit(EXIT_FAILURE);
    }

    r = munmap(p, one_hundred_mb);
    if (r == -1) {
        perror("munmap failed to deallocate memory");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

// max_sbrk tests how many MB can sbrk provide.
void max_sbrk()
{
    // Stores value of one megabyte.
    intptr_t one_mb = 1024*1024;
    // Pointer to the allocated memory address.
    void* p;

    for (intptr_t i = 1;; i++) {
        p = sbrk(one_mb*i);
        if (p == (void*)-1) {
            perror("sbrk failed to allocate more memory");
            exit(EXIT_FAILURE);
        }

        p = sbrk(-1*one_mb*i);
        if (p == (void*)-1) {
            perror("sbrk failed to deallocate memory");
            exit(EXIT_FAILURE);
        }

        printf("%ldMB\n", i);
    }
    
    exit(EXIT_SUCCESS);
}

// MB_100_sbrk is for testing how fast can sbrk provide 100MB of memory.
// Test this function with program (time).
void MB_100_sbrk()
{
    // Stores value of one hundred megabyte.
    intptr_t one_hundred_mb = 1024*1024*100;
    // Pointer to the allocated memory address.
    void* p;

    p = sbrk(one_hundred_mb);
    if (p == (void*)-1) {
        perror("sbrk failed to allocate 100MB of memory");
        exit(EXIT_FAILURE);
    }

    // Freeing memory.
    p = sbrk(one_hundred_mb);
    if (p == (void*)-1) {
        perror("sbrk failed to deallocate memory");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

// test_static_memory is for checking how this program uses static memory.
// Basically, ...
void test_static_memory()
{
    // Setting explicitly array size.
    int arr_size = 10000;
    //
    void* before;
    //
    void* after;
    
    //
    before = sbrk(0);
    if (before == (void*)-1) {
        perror("sbrk failed to get current address");
        exit(EXIT_FAILURE);
    }

    printf("Before: %p\n", before);

    // Creating and initializing elements of the array.
    char arr[arr_size];
    for (int i = 0; i < arr_size; i++) {
        arr[i] = 'W';
    }

    //
    after = sbrk(0);
    if (after == (void*)-1) {
        perror("sbrk failed to get current address");
        exit(EXIT_FAILURE);
    }

    printf("After: %p\n", after);

    // 
    int res = (after - before)/(1024);
    printf("KB allocated: %d\n", res);

    exit(EXIT_SUCCESS);
}

int main(void)
{
    // --- Test malloc() ---
    // max_malloc();
    // MB_100_malloc();

    // --- Test mmap() ---
    // max_mmap();
    // MB_100_mmap();

    // --- Test sbrk() ---
    // max_sbrk();
    // MB_100_sbrk();

    test_static_memory();

    // Exit gratefully if later main function gets modified.
    exit(EXIT_SUCCESS);
}

// ===================================
// Q&A
// ===================================
// Katram veidam izpētīt:
// 1. Kāds ir maksimālais atmiņas daudzums, ko tādā veidā var alocēt?
// 2. Kāds programmas darbības laiks, ja tiek rezervēti 100 megabaiti atmiņas?

// For malloc():
// 1. 3652MB (the result varies)
// 2. 0m0.003s

// For mmap():
// 1. 3738MB (the result varies)
// 2. 0m0.003s

// For sbrk():
// 1. 3708MB (the result varies)
// 2. 0m0.003s

// With the help of size I got the following information: (or could use objdump)
//    text    data     bss     dec     hex filename
//    3143     648       8    3799     ed7 md4
// * the output is very similar whether the functions are commented or not.