#include <string.h>
#include <stdio.h>
#include <unistd.h>

void main(void)
{
    char buf1[12] = "tests\n";
    char buf2[12];

    int *fd0 = open("fails",O_RDWR|O_CREAT);
    int *fd1 = open("fails",O_RDWR);

    write(fd0, buf1, strlen(buf1));
    write(1, buf2, read(fd1, buf2, 12));
    close(fd0);
    close(fd1);
}