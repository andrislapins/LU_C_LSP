#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloca.h>

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

    // exit(EXIT_SUCCESS);
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

    // exit(EXIT_SUCCESS);
}

// calloc_100_mb is for testing how program allocates memory and initializes it.
void calloc_100_mb()
{
    // Stores value of one hundred megabyte.
    size_t one_hundred_mb = 1024*1024*100;

    void* p = (void*)calloc(one_hundred_mb, 1);
    if (p == NULL) {
        perror("calloc failed to allocate 100MB of memory");
        exit(EXIT_FAILURE);
    }

    free(p);

    // exit(EXIT_SUCCESS);
}

void max_stack_alloca(size_t i)
{
    // Stores value of one megabyte.
    size_t one_mb = 1024*1024;
    // Pointer to the allocated memory address.
    void* p;

    p = (void*)alloca(one_mb*i);
    if (p == NULL) {
        perror("calloc failed to allocate 100MB of memory");
        exit(EXIT_FAILURE);
    }
}

void test_large_array()
{
    char array[100 * 1024 * 1024];
    memset(array, 0, sizeof(array));
}

void test_large_array_static()
{
    static char array[100 * 1024 * 1024];
    memset(array, 0, sizeof(array));
}

int main(void)
{
    printf("allocating...\n");

    // Have only one function uncommented which to test.
    // max_malloc();
    // MB_100_malloc();
    // calloc_100_mb();
    // for (size_t i = 1;; i++) {
    //     max_stack_alloca(i);

    //     printf("%ldMB\n", i);
    // }
    // test_large_array();
    test_large_array_static;

    // To check attributes of the process by commmand top. CTRL+C to interrupt 
    // the process.
    // while (1);

    return 0;
}

// Eksperiments ar max_malloc() jeb rezervēt atmiņu pa 1MB.
// real    0m0.086s
// user    0m0.024s
// sys     0m0.062s
// Memory: 5522MB
// Strace statistics:
// % time     seconds  usecs/call     calls    errors syscall
// ------ ----------- ----------- --------- --------- ----------------
//  55.78    0.054486          10      5514           munmap
//  24.90    0.024322           4      5515           write
//  19.30    0.018857           3      5520         2 mmap
//   0.01    0.000005           1         4           brk
//   0.00    0.000004           1         3           close
//   0.00    0.000004           1         4           fstat
//   0.00    0.000004           4         1           dup
//   0.00    0.000004           4         1           fcntl
//   0.00    0.000000           0         1           read
//   0.00    0.000000           0         4           mprotect
//   0.00    0.000000           0         3         3 access
//   0.00    0.000000           0         1           execve
//   0.00    0.000000           0         1           arch_prctl
//   0.00    0.000000           0         2           openat
// ------ ----------- ----------- --------- --------- ----------------
// 100.00    0.097686                 16574         5 total

// Eksperiments ar MB_100_malloc() jeb rezervēt atmiņu vienā gabalā.
// real    0m0.003s
// user    0m0.001s
// sys     0m0.003s
// Memory: 100MB
// Strace statistics:
// % time     seconds  usecs/call     calls    errors syscall
// ------ ----------- ----------- --------- --------- ----------------
//  24.66    0.000238          40         6           mmap
//  18.86    0.000182          91         2           munmap
//  10.67    0.000103          34         3         3 access
//  10.36    0.000100          50         2           openat
//   8.50    0.000082          27         3           fstat
//   8.19    0.000079          40         2           close
//   6.63    0.000064          64         1           execve
//   5.91    0.000057          14         4           mprotect
//   2.59    0.000025           8         3           brk
//   2.49    0.000024          24         1           write
//   0.83    0.000008           8         1           read
//   0.31    0.000003           3         1           arch_prctl
// ------ ----------- ----------- --------- --------- ----------------
// 100.00    0.000965                    29         3 total

// Secinājums: Rezervējot datus pa gabaliem tiek veikti 571x mazāk sistēmu
// izsaukumu. Tādējādi rezervēt atmiņu pa lielākiem gabaliem ir daudz izdevīgāk.

// ====================================================================

// Novērtējums - atmiņa tiek rezervēta, bet netiek izmantota. 
// Pēc eksperimenta ar MB_100_malloc():
// real    0m0.003s
// user    0m0.001s
// sys     0m0.003s
// VmSize:	    4508 kB // sum of all mapped memory.
// VmRSS:	    1260 kB // physical memory used.

// Novērtējums - atmiņa tiek rezervēta, un tiek izmantota. 
// Ar funkciju calloc_100_mb().
// real    0m0.003s
// user    0m0.003s
// sys     0m0.000s
// Strace statistics:
// % time     seconds  usecs/call     calls    errors syscall
// ------ ----------- ----------- --------- --------- ----------------
//  16.73    0.000135          23         6           mmap
//  14.87    0.000120          30         4           mprotect
//  12.89    0.000104         104         1           read
//  12.89    0.000104          35         3         3 access
//   9.29    0.000075          75         1           execve
//   8.55    0.000069          35         2           openat
//   6.94    0.000056          28         2           munmap
//   6.20    0.000050          17         3           fstat
//   6.20    0.000050          17         3           brk
//   3.84    0.000031          16         2           close
//   1.61    0.000013          13         1           arch_prctl
//   0.00    0.000000           0         1           write
// ------ ----------- ----------- --------- --------- ----------------
// 100.00    0.000807                    29         3 total
// VmSize:	    4508 kB
// VmRSS:	    1400 kB

// Secinājums: Secinu, ka nodalītā atmiņa rezervējot 100MB ir vienāda starp 
// malloc un calloc, bet tā starpība VmRSS lielumā starp funckijām ir
// calloc datu inicializācijas dēļ, kas tikuši ierakstīti fiziskajā atmiņā.

// Steka atmiņa. alloca() nebeidzami ciklējas. Šī neparedzamā darbība ir jau
// pamatota alloca "manual page" skaidrojumos, ka alloca var "overflow'ot".

// test_large_array izmet Segmentation fault (core dumped).

// test_large_array_static funckijas masīvs tiktu glabāts, nevis BSS segmentā,
// bet gan inicializēto datu segmentā, kas ir daļa no 'datu segmenta'.