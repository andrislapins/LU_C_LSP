#include <stdio.h>
#include <math.h>

int main () {
   double x, ret;
   int n;

   x = 0.6532;
   n = 3;
   ret = ldexp(x ,n);
   printf("%f * 2^%d = %f\n", x, n, ret);
   
   return(0);
}