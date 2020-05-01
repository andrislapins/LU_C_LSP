#include <stdio.h>

#define NUM_LOOPS 50

struct counters {
    int little:2;
    int medium:4;
    int large:6;
};

struct u_counters {
    unsigned int little:2;
    unsigned int medium:4;
    unsigned int large:6;
};

struct u_counters_packed {
    unsigned int little:2;
    unsigned int medium:4;
    unsigned int large:6;
}__attribute__((packed));

int main() {
    struct counters counts;

    counts.little = 0;
    counts.medium = 0;
    counts.large = 0;

    for (int i = 0; i < NUM_LOOPS; i++) {
        counts.little++;
        counts.medium++;
        counts.large++;
        
        printf("%d, %d, %d\n", counts.little, counts.medium, counts.large);
    }

    printf("sizeof counts = %lu\n", sizeof(counts));
}