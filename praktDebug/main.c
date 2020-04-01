#include <stdio.h>
#include <stdlib.h>

void nullDeref(int *p) {
    *p = 0x1234;
}

void f(int param);
void g(void) {
    printf("calling g\n");
    f(0);
}

void f(int param) {
    int localVariable = 1;
    printf("calling f, &localVariable=%p\n", &localVariable);
    g();
}

void stackOverflow(void) {
    f(1);
}

void memoryAllocate(void) {
    int i, j;
    char *array = malloc(10);
    i = j = 5;
    while (i-- >= 0) {
        array[i] = 0x13;
    }
    while (j++ >= 0) {
        array[j] = 0x13;
    }
}


int main(void) {
    int i;
    int *p = NULL;
    nullDeref(p);
    stackOverflow();
    for (i = 0; i < 100; ++i) {
        memoryAllocate();
    }
    return 0;
}