#include <stdio.h>

void main(void)
{
    int mas[5] = {1,2,3,4,5};

    printf("1.%p == %p\n", &mas, &mas[0]); //Abas adreses vienādas
    printf("2.%p =? %p\n", &mas[1], (int*)((unsigned long)&mas+1)); //Ups
    printf("3.%i =? %i\n", mas[1], *(int*)((unsigned long)&mas+1));
    printf("4.%p == %p\n", &mas[1], (int*)((unsigned long)&mas+1*sizeof(int)));
    printf("5.%i == %i\n", mas[1], *(int*)((unsigned long)&mas+1*sizeof(int)));
    printf("6.%i == %i\n\n", mas[1], *(mas+1));

    printf("7.%p\n", mas);
    printf("8.%d\n", *mas);
    printf("9.%p\n", mas + 1);
    printf("10.%d\n", *(mas + 1));
    printf("11.%p\n", &(mas[0]) );
    printf("12.%p\n", &(mas[1]) );
    printf("13.%d\n", *&(mas[0]) );
    // printf("%p", **(mas[0]) );
}