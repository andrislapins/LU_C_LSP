#include <stdio.h>
#include <math.h>

void main() {
    int angle_degree;
    double angle_radian, pi, value;

    printf("\nCompute table\n\n");

    pi = 4.0*atan(1.0);
    printf("Value of PI = %f \n\n", pi);

    angle_degree=0;

    while (angle_degree <= 360)
    {
        angle_radian = pi * angle_degree/180.0;
        value = sin(angle_radian);
        printf("%3d     %f \n", angle_degree, value);

        angle_degree = angle_degree + 10;
    }
}