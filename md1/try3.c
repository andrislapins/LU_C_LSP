#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Node {
    char name[57];
    struct Node *father;
    struct Node *mother;

    int weight;

    // Node children[16];

    int visited;
};

int main(void) 
{
    struct Node *people[1024] = { NULL };
    struct Node *people2[1024] = { NULL };
    int count_of_people = 0;
    int global_weight = 0;

    int result;

    struct Node *pob; // Person Of Block

    char line[64];
    char label[6], data[57];

    while (fgets(line, 64, stdin) != NULL)
    {
        // --- Error checking ---
        if (line[0] == '\n') continue;

        result = sscanf(line, "%s %s", label, data);
        if (result == EOF)
        {
            printf("Could not scan a line in your data\n");
            return 1;
        }

        result = printf("%s %s\n", label, data);
        if (result < 0) 
        {
            printf("Failed to print a scanned line");
            return 1;
        }

        // --- Managing nodes ---

        struct Node *person = malloc(sizeof *person);
        person->weight = 0;
        for (int i = 0; i < 16; i++)
        {
            // person->children[i] = NULL;
        }

        if (strcmp(label, "VARDS") == 0) 
        {
            strcpy(person->name, data);
            pob = person;
        }

        if (strcmp(label, "TEVS") == 0)
        {
            strcpy(person->name, data);
            person->weight++;
            for (int i = 0; i <= 16; i++)
            {
                // if (person->children[i] == NULL)
                // {
                //     person->children[i] = pob;
                //     break;
                // }
            }
            pob->father = person;
        }

        if (strcmp(label, "MATE") == 0)
        {
            strcpy(person->name, data);
            person->weight++; 
            for (int i = 0; i <= 16; i++)
            {
                // if (person->children[i] == NULL)
                // {
                //     person->children[i] = pob;
                //     break;
                // }
            }
            pob->mother = person;
        }

        for (int i = 0; i < 1024; i++)
        {
            if (people[i] == NULL) 
            {
                people[i] = person;
                printf("w: %d\n", person->weight);
                break;
            }
        }

        if (person->weight > global_weight) global_weight = person->weight;
        count_of_people++;
    } // End of reading lines.


    // --- Printing out the family tree (sequence) ---
    int j = 0;
    while(global_weight != 0) 
    {
        for(int i = 0; i < 1024; i++)
        {
            // if (people[i]->weight == global_weight)
            // {
                people2[j] = people[i];
                j++;
            // }
        }

        printf("j: %d", j);

        global_weight = global_weight - 1;
    }

    // people2[j] = '\0';


    for (int i = 0; i < j-1; i++)
    {
        printf("%s", people2[i]->name);
    }

    printf("\n");
    return 0;
}