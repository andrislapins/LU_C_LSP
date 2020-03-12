#define POSIX_C_SOURCE 200809L

#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

jmp_buf buffer;

void alarm_handler(int sig)
{
    printf("User defined alarm: %d\n", sig);
    longjmp(buffer, 1);
}

int main(void)
{
    signal(SIGALRM, alarm_handler);

    if (setjmp(buffer)) {
        return 0;
    }

    // =====================================================
    // Timer set to three seconds.
    alarm(3);
    // Infinite loop.
    while(1);
    // Return non-zero value so compiler would not complain.
    return -1;
}