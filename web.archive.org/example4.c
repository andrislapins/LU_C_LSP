#include <stdio.h>

void main() {
    char text_1[100], text_2[100], text_3[100];
    char *ta, *tb;
    int i;

    char message[] = "Hello, I am a string!";

    printf("Original message: %s\n", message);

    i=0;
    while( (text_1[i] = message[i]) != '\0' ) // How the hell this works?

    printf("Mine: %c", text_1[1] = message[1]);

    printf("Text 1: %s\n", text_1);


    ta=message;
    tb=text_2;
    while ( ( *tb++ = *ta++ ) != '\0' )

    printf("Text 2: %s\n", text_2);
}