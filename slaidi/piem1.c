#include <stdio.h>

int g = 1;

void foo(void) 
{
    printf("in foo(), g=%d\n", g);
}

void bar(void) 
{
    static int g;

    g++;
    printf("in bar(), g=%d\n", g);
}
int main(void) 
{
    int g = 2;
    printf("in main(), g=%d\n", g);

    foo(); bar();
    {
        auto const int g = 3;
        printf("in main() again, g=%d\n", g);
        foo(); bar();
    }

    printf("Size of int: %lu", sizeof(int));

    return 0;
}