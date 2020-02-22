/**
 * Author: Andris Lapins, al18011
 */

#include <stdio.h>
#include <string.h>

int md5Flag; // Boolean to check if file comparison by MD5 hash is needed.
int dateFlag; // Boolean to check if file comparison by date is needed.

void printHelpText(void);

int main(int argc, char** argv)
{
    md5Flag = 0;
    dateFlag = 0;

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

    //

    printf("\n");
    return 0;
}

void printHelpText()
{
    printf("Help text\n");
}