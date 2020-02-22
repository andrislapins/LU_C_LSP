#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>

void list_directories(char*, char*);

int result;

int main(int argc, char **argv)
{
    char* file = argv[1];
    char* directory = argv[2];

    if (argc != 3)
    {
        printf("You have given incorrect number of arguments\n");
        return -1;
    }

    list_directories(file, directory);    

    return result;
}

void list_directories(char* file, char* directory)
{
    DIR *d;

    d = opendir(directory);
    if (!d)
    {
        printf("An error occurred opending the directory\n");
        result = -1;
        return;
    }

    while (1)
    {
        struct dirent *entry;
        struct stat buf;
        char* d_name; 

        entry = readdir(d);
        if (!entry) break;

        d_name = entry->d_name;

        printf("%s/%s\n", directory, d_name);

        if (stat(directory, &buf) != 0)
        {
            printf("Could not stat - %s", directory);
            // perror("couldn't stat");
            result = -1;
            return;
        }

        /* Check that the directory is not "d" or d's parent. */
        if (!(S_ISDIR(buf.st_mode)))
        {
            if (strcmp (d_name, "..") != 0 &&
                strcmp (d_name, ".") != 0) {
                int path_length;
                char path[255];

                path_length = snprintf (
                    path, 255, "%s/%s", directory, d_name
                );
                printf ("%s\n", path);
                if (path_length >= 255) {
                    printf("Path length has got too long.\n");
                    result = -1;
                    return;
                    // exit (EXIT_FAILURE);
                }
                /* Recursively call "list_directories" with the new path. */
                list_directories(file, path);
            }
        }
    }
    
    /* After going through all the entries, close the directory. */
    if (closedir (d)) {
        fprintf (
            stderr, "Could not close '%s': %s\n", directory, strerror (errno)
        );
        // exit (EXIT_FAILURE);
        result = -1;
        return;
    }

    result = 0;
    return;
}