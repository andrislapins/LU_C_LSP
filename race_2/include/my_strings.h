#ifndef _MY_STRINGS_H_
#define _MY_STRINGS_H_

// Fully copy to dest from src by giving the "size of" src string.
void string_n_copy(char *dest, char *src, int n);
// Get the length of characters (which are not \0).
int string_length(char *str);
// Fill str with zeros of length len + 1.
void string_zero(char *str, int len);

#endif