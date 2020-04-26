#include "../include/my_strings.h"

void string_n_copy(char *dest, char *src, int n) {
    int i;

    for(i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

int string_length(char *str) {
    int i = 0, count = 0;

    while(str[i++] != '\0') {
        count += 1;
    }

    return count;
}

void string_zero(char *str, int len) {
    for(int i = 0; i < len; i++) {
        str[i] = '\0';
    }
}