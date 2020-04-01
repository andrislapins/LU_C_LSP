/**
 * Author: Andris Lapiņš, al18011
 * Description: Given the size of the static global array, I use as a limited
 * space of memory to test my own made malloc function (alloc and free)
 * implementations.
 * Last compiled/modified: 24.03.2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

// The range of bytes for my pool of memory.
#define MY_POOL_SIZE 88 // default: 4096
// Char value for indicating that block of memory HAS NOT been freed.
#define NOT_FREE 48
// Char value for indicating that block of memory HAS been freed.
#define FREE 49

// My pool of memory with which to experiment my malloc function implementation.
unsigned char pool[MY_POOL_SIZE];

// Structure of a block of memory.
// [size(8 bytes)][free(1 byte)][data(n bytes)]

// myalloc is my implementation of malloc() for static arrays.
void* myalloc(size_t size)
{
    // To store the position of the end of the last allocated block in form of
    // array of char (bytes).
    unsigned char index_buf[sizeof(int)];
    // The position of the end of the last allocated block.
    int index;
    // Buffer storing the value of size_t decimal in array of unsigned char.
    unsigned char size_buf[sizeof(size_t)];
    // The decimal value of the next position for allocation.
    size_t next_pos;
    // To accumulate the available size if many small chunks in sum have enough
    // available memory.
    size_t free_size = 0;
    // This helps to check if by searching free gaps in pool of mem we have made
    // a full loop over the mem pool.
    size_t temp_pos = next_pos;

    // Get the position of the the end of the last allocated chunk.
    memcpy(index_buf, pool, sizeof(int));
    next_pos = *(int*)(index_buf);

    // For the first mem allocation move the "pointer" in the array 4 bytes
    // from the beginning to leave space for storing the index of the end
    // of last allocated block.
    // OR
    // In case at the end of the pool of mem. 9 is the size of the metadata
    // structure. 
    if (next_pos == 0 || next_pos + 9 >= MY_POOL_SIZE)
    {
        next_pos = 4;
    }

    // For debuf purposes - to know at what index the allocation starts.
    printf("Next Position: %ld\n", next_pos);

    /* The search for free gaps */
    unsigned char new_pos = pool[next_pos + sizeof(size_t) + 1];
    if (new_pos != 0 || new_pos == FREE)
    {
        while(1)
        {
            if (new_pos == FREE)
            {
                for (int i = 0; i < sizeof(size_t); i++)
                {
                    size_buf[i] = pool[next_pos + i];
                }
                
                free_size += *(size_t*)size_buf;

                if (free_size >= size)
                {
                    break;
                }
            }
        }
    }

    // Get the value of casting to array of unsigned char from size_t.
    memcpy(size_buf, (unsigned char*)&size, sizeof(size_t));

    // Each index in range of size_t from start of block of memory is a byte
    // of the full size_t value.
    for (int i = 0; i < sizeof(size_t); i++)
    {
        pool[next_pos + i] = size_buf[i];
    }

    // Set the "free" area in metadata as NOT freed.
    pool[next_pos + sizeof(size_t) + 1] = NOT_FREE;

    // Store the value of the end of the last allocated memory from where to 
    // find the next_fit at the start of the pool.
    // pool[0-3] == next_pos
    index = next_pos + sizeof(size_t) + 1 + size;
    memcpy(index_buf, (unsigned char*)&index, sizeof(int));
    for (int i = 0; i < sizeof(int); i++)
    {
        pool[i] = index_buf[i];
    }

    // Return the location of allocated space where data are allowed to 
    // be modified.
    return (void*)&pool[next_pos + sizeof(size_t) + 2];
}

// myfree is my implementation of malloc's free() func for static arrays.
int myfree(void* ptr)
{
    int index = ptr - (void*)pool;
    printf("Removed a block of memory starting at index %d\n", index);

    // Set the "free" area as IS freed.
    pool[index - 1] = FREE;
}

// For checking the initialization of elements in the mem pool.
void show_memory()
{
    printf("=== MEMORY ===\n");
    for (int i = 0; i < MY_POOL_SIZE; i++)
    {
        printf("%d ", pool[i]);
    }

    printf("\n");
}

void main()
{
    // Pointer to the allocated memory.
    void* myptr;
    void* myptr_2;
    void* myptr_3;
    // To store the result of myfree function.
    int res;
    // To store the ammount of char elements of array to allocate.
    size_t size;

    size = 32;
    myptr = myalloc(size);
    if (myptr == NULL)
    {
        printf("Could not allocate %ld bytes of memory\n", size);
        exit(EXIT_FAILURE);
    }

    show_memory();

    size = 32;
    myptr_3 = myalloc(size);
    if (myptr == NULL)
    {
        printf("Could not allocate %ld bytes of memory\n", size);
        exit(EXIT_FAILURE);
    }

    show_memory();

    res = myfree(myptr);
    if (res == -1)
    {
        printf("Could not free memory\n");
        exit(EXIT_FAILURE);
    }

    show_memory();

    size = 16;
    myptr_2 = myalloc(size);
    if (myptr_2 == NULL)
    {
        printf("Could not allocate %ld bytes of memory\n", size);
        exit(EXIT_FAILURE);
    }

    show_memory();

    res = myfree(myptr_2);
    if (res == -1)
    {
        printf("Could not free memory\n");
        exit(EXIT_FAILURE);
    }

    show_memory();

    exit(EXIT_SUCCESS);
}

// Todo list;
// + allocate memory 2 times a row
// + free memory
// + check for available free memory gaps
// + ... and assign block of mem in it
// [] return errors

/** === Review of the problems in my code ===
 * Starting to develope the block of code starting on line 64 for dealing
 * with freed memory chunks I understood that there are a few problems to
 * develope a fully functional allocation function:
 * (explanations are given by examples)
 * 
 * 1. In case of having a 32 byte chunk freed and after that allocating a new
 * block of 16 bytes in that same freed chunk, after the 16 byte allocated chunk
 * the code is not checking whether after the 32 byte freed chunk there is an
 * another allocated chunk. Therefore this "another" chunk would be overwritten.
 * Solution: I have thought of many solutions but all of them have some flaw.
 * 
 * 2. I wanted to implement code that would check if the "allocation pointer"
 * (next_pos) has made a loop over the pool of memory. This would be easily
 * implemented if I could tackle the 1. problem in the list of my code review.
 * 
 * 3. As I assume, I could internally store values of size int not size size_t
 * to save space internally.
 * 
 * 4. In some places, by looking at the printed memory output it seems some
 * allocations have been misplaced by one char further or closer to the start
 * of chunk. 
 */