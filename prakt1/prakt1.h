#ifndef PRAKT1
#define PRAKT1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int sv_garums(char *virkne)
{
    return strlen(virkne) + 1;
}

void sv_kopet(char *no, char *uz)
{
    strcpy(uz, no);
}

int sv_meklet(char *kur, char *ko)
{
    char *rez = strstr(kur, ko); // iegut pointeri, kur atrodas "ko" apakš-strings.
    if (!rez) return -1; // atgreizt -1, ja netika atrasts.
    return rez - kur; // atnemt pointeri no pointera, lai iegūtu indeksu.
}

void sv_apgriezt(char *virkne)
{
    // Palīdzību sniedza Edvards Bukovskis.
    char *aste = virkne; // Iegūstu pointeri uz padotās virknes sākumu.
    while(*aste) 
    {
        // printf("%c\n",*aste); // Atgriež value ar šo pointera referenci.
        ++aste; //  Pa pointera vienībām uz augšu.
    }
    --aste;

    for (; virkne < aste; virkne++, --aste)
    {
        char v = *virkne, t = *aste;
        *virkne = t;
        *aste = v;
    }
}

// Test uzdevumam PD1.5:
// Segmentation fault (core dumped)
// Bija vairāki varianti kā šo izpildīt, bet galvenā problēma
// ir tas, ka nevaru padot array uz strcopy un visām iepriekšējām
// vēlamajām funkcijām.
void sv_vapgriezt(char *virkne)
{
    char tempArr[20];
    char *tempPtr;
    char delim[] = " ";
    char *ptr = strtok(virkne, delim);

    char *aste = ptr; 
    while(*aste) ++aste;
    --aste;

    for (; virkne < aste; virkne++, --aste)
    {
        char v = *virkne, t = *aste;
        *virkne = t;
        *aste = v;
    }
    
    while (ptr != NULL)
    {
        ptr = strtok(NULL, delim);

        char *aste = ptr; 
        while(*aste) ++aste;
        --aste;

        for (; virkne < aste; virkne++, --aste)
        {
            char v = *virkne, t = *aste;
            *virkne = t;
            *aste = v;
        }
    }
}

// int main(void) 
// {
//     char bufferis[20];

//     printf("Tests uzdevumam PD1.1:\n");
//     printf("%d\n", sv_garums("hello world"));
//     printf("%d\n", sv_garums("123"));
//     printf("%d\n", sv_garums(""));

//     printf("\nTests uzdevumam PD1.2:\n");
//     sv_kopet("hello world", bufferis);
//     printf("%s\n", bufferis);
//     sv_kopet("123", bufferis);
//     printf("%s\n", bufferis);
//     sv_kopet("", bufferis);
//     printf("%s\n", bufferis);

//     printf("\nTests uzdevumam PD1.3:\n");
//     printf("%d\n", sv_meklet("hello world", "or"));
//     printf("%d\n", sv_meklet("123", "3"));
//     printf("%d\n", sv_meklet("", "none"));

//     printf("\nTests uzdevumam PD1.4:\n");
//     sv_kopet("abc", bufferis);
//     sv_apgriezt(bufferis);
//     printf("%s\n", bufferis);    
    
//     printf("\nTest uzdevumam PD1.5:\n");
//     sv_kopet("hello world", bufferis);
//     sv_vapgriezt(bufferis);
//     printf("%s\n", bufferis);
// }


#endif