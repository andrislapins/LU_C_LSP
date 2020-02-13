#include <stdio.h>

void main() 
{
    FILE *fp;
    int i;

    fp = fopen("foo.dat", "a");

    fprintf(fp, "\nSample Code\n\n");
    for (i = 1; i <= 10; i++)
        fprintf(fp, "i = %d\n", i);

    fclose(fp);
}