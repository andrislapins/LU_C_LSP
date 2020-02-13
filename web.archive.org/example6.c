#include <stdio.h>

void main()
{
    int i, nc;

    nc = 0;
    i = getchar();
    while (i != EOF) {
        nc = nc + 1;
        i = getchar();
    }

    printf("Number of characters in file = %d\n", nc);
}

// in terminal - ctrl + D for EOF
