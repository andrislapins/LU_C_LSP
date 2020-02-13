/*
 * Developed by Andris Lapins, al18011
 * Last time modified/compiled: 13.02.2020
 * 
 * Description:
 * This program copies the contect of the file as the first argument of the
 * command to the file as the second argument.
*/

#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    FILE *in_file; // Input file pointer.
    FILE *out_file; // Output file pointer
    int c; // To read the input character in case of already existent 
           // output file.
    int pos; // To decrement by character from the end of  input file
             // until zero (the end of reading-writing to output file).
    char ch; // To read the next character form the input file.

    // Open and test the input file.
    in_file = fopen(argv[1], "r");
    if (in_file == NULL)
    {
        printf("Error! Could not open file %s\n", argv[1]);
        return -1;
    }

    // Manage whether the given output file exists.
    if (access(argv[2], F_OK) != -1)
    {
        printf("The given output file already exists!\n");
        printf("Do you want to overwrite it? (y/n): ");
        c = getchar();
        if (c == 110) // In case of "n*"
        {
            printf("Okay. Goodbye!\n");
            fclose(in_file);
            return 0;
        }
        if (c != 121) // In case of not having "y*".
        {
            printf("Did not understand what you meant. ");
            printf("Aborting the program!\n");
            return -1;
        }
    }


    // Open and test the output file.
    out_file = fopen(argv[2], "w");
    if (out_file == NULL)
    {
        printf("Error! Could not open file %s", argv[2]);
        fclose(in_file); // Because opening previously the input file succeeded.
        return -1;
    }

    // Manage copying from the input file to output file.
    // The code below was taken from https://www.sanfoundry.com/c-program-copy-file/
    fseek(in_file, 0L, SEEK_END);
    pos = ftell(in_file);
    fseek(in_file, 0L, SEEK_SET); // Return to the start of input file.
    while (pos--)
    {
        ch = fgetc(in_file); // Get the next charcter.
        fputc(ch, out_file); // Write the given character to output file.
    }

    // Program ran successfuly.
    fclose(in_file);
    fclose(out_file);
    return 0;
}