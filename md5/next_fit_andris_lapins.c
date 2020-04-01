/**
 * Author: Andris Lapiņš, al18011
 * Description: Implementation of Next Fit algorithm for 5. homework of LSP-b.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    FILE *fp_chunks, *fp_request;
    int i;
    int sum = 0;
    int read_buf_size = 8;
    char read_buf[read_buf_size];
    void *pool;

    for (i = 0; i < argc; i++) {
        if (strcmp(argv[i],"-c") == 0) {
            fp_chunks = fopen(argv[i+1], "r");
            if (fp_chunks == NULL) {
                perror("Could not open chunk file");
                exit(EXIT_FAILURE);
            }
        }

        if (strcmp(argv[i],"-s") == 0) {
            fp_request = fopen(argv[i+1], "r");
            if (fp_request == NULL) {
                perror("Could not open request file");
                exit(EXIT_FAILURE);
            }
        }
    }

    while (fgets(read_buf, read_buf_size, fp_chunks)) {
        printf("%s", read_buf);
        sum += strtol(read_buf, NULL, 10);
    }

    printf("Sum: %d\n", sum);

    pool = malloc(sum);

    free(pool);
    return EXIT_SUCCESS;
}