/**
 *  Author: Andris Lapiņš, al18011
 *  Last modified/compiled: 26.03.2020
 *  Description: http://selavo.lv/wiki/index.php/LU-LSP-b:L08
 *   Programma kas taisa N pavedienus, kur katrs izdrukā M burtus.
 *   N un M ir parametri. Drukājamais burts ir pēc izvēles. Katrs pavediens var drukāt savu burtu, vai visi vienu.
 *   Testēt un novērot gan kādā kārtībā burti tiek drukāti (ja dažādi), gan kopējo burtu skaitu.
 *   Iesūtīt programmas kodu un secinājumus.
 *  References:
 *   https://www.geeksforgeeks.org/thread-functions-in-c-c/
 *   https://stackoverflow.com/questions/11624545/how-to-make-main-thread-wait-for-all-child-threads-finish
 *   https://stackoverflow.com/questions/20196121/passing-struct-to-pthread-as-an-argument
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct Values{
    int number;
    char letter;
} Values;

void* entry_point(void* arg)
{
    // Convert the argument back to struct Values.
    Values* values = (Values*)arg;

    printf(
        "Hello from the %d thread having a letter %c\n", 
        (*values).number, (*values).letter
    );

    free(values);
    return NULL;
}

int main(int argc, char** argv)
{
    // The number of threads.
    int N = 3;
    // The symbol to print and icrement.
    char M = 'a';
    // An array of threads.
    pthread_t threads[N];
    // Struct where to store the values for a thread.
    Values* values;

    for (int i = 0; i < N; i++) {
        values = malloc(sizeof(Values));
        (*values).number = i;
        (*values).letter = M+i;

        pthread_create(&threads[i], NULL, entry_point, (void*)values);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}

// Conclusion and Review.
// The ammount of threads and the kind of letters they print (in this case,
// from which letter to start incrementing) is hard-coded. The order of the
// threads by which the output their text differs almost every third time.
// (My output for comparison):
// .../Linux/prakt8$ ./pd8
// Hello from the 1 thread having a letter b
// Hello from the 0 thread having a letter a
// Hello from the 2 thread having a letter c
// .../Linux/prakt8$ ./pd8
// Hello from the 0 thread having a letter a
// Hello from the 1 thread having a letter b
// Hello from the 2 thread having a letter c
// .../Linux/prakt8$ ./pd8
// Hello from the 0 thread having a letter a
// Hello from the 1 thread having a letter b
// Hello from the 2 thread having a letter c

// The order of the output is not in the same order as the order of creating
// the threads sometimes is because I at first create them all in order at first
// and only then wait them to finish (and output them according to as OS has
// scheduled them).