/**
 * Author: Andris Lapiņš, al18011
 * Description: This program cypher the given text by chacacter. Program
 * reaction is dependent on the given flags.
 * Last time compiled/modified: 05.03.2020
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
    // Default values where to input and output characters.
    FILE* output_stream = stdout;
    FILE* input_stream = stdin;
    // Storing boolean whether certain flags have been detected.
    int o_Flag = 0;
    int t_Flag = 0;
    int s_Flag = 0;
    int input_Flag = 0;
    // Store the path to the file according to certain flags.
    char t_File[256];
    char s_File[256];
    char o_File[256];
    char input_File[256];
    // ch stores regular given character.
    int ch = 0;
    // res stores the printing function's returned value.
    int res;
    // cyphered_char stores a character after XOR cypher with 0x01
    int cyphered_char;

    // Checking the flags passed to the command of this program.
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0) { // Output file.
            o_Flag = 1;
            i++;
            for (int j = 0; j < strlen(argv[i]); j++) {
                o_File[j] = argv[i][j];
            }
        } else if (strcmp(argv[i], "-t") == 0) { // Translation file.
            t_Flag = 1;
            i++;
            for (int j = 0; j < strlen(argv[i]); j++) {
                t_File[j] = argv[i][j];
            }
        } else if (strcmp(argv[i], "-s") == 0) { // Cypher table.
            s_Flag = 1;
            i++;
            for (int j = 0; j < strlen(argv[i]); j++) {
                s_File[j] = argv[i][j];
            }
        } else { // Input file.
            input_Flag = 1;
            for (int j = 0; j < strlen(argv[i]); j++) {
                input_File[j] = argv[i][j];
                printf("%c", input_File[j]);
            }
        }
    }

    // If output file has been specified.
    if (o_Flag) {
        output_stream = fopen(o_File, "w");
        if (input_stream == NULL) {
            perror("Error on opening ouput file");
            exit(EXIT_FAILURE);
        }
    }

    // If input file has been specified.
    if (input_Flag) {
        input_stream = fopen(input_File, "r");
        if (input_stream == NULL) {
            perror("Error on opening input file");
            exit(EXIT_FAILURE);
        }
    }

    // Managing character cyphering.
    while((ch = fgetc(input_stream)) != EOF) {
        if (ch == '\n') continue;

        // Cypher the given character with XOR.
        cyphered_char = ch ^ 0x01;

        res = fprintf(output_stream, "%c", cyphered_char);
        if (res < 0) {
            printf("Could not print character from input\n");
            exit(EXIT_FAILURE);
        }
    }

    fclose(input_stream);
    fclose(output_stream);
    printf("\n");
    exit(EXIT_SUCCESS);
}