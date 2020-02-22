#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Node {
    char name[57];
    int weight;
    int visited;
    struct Node *father;
    struct Node *mother;
    struct Node *children[16];
};

struct Node *people[1024] = { NULL };
struct Node *pob = NULL; // Hold pointer to the first person of a block.

int count_of_people = 0; // Hold the count to restrict looping all 1024 times??
int global_weight = 0;
int result;
int newPerson = 1;
int parentIsKnown = 0;
int incrementAdded = 0;

char line[64];
char label[6], data[57];

int main(void) 
{
    while (fgets(line, 64, stdin) != NULL)
    {
        // --- Error checking ---
        if (line[0] == '\n' || line[0] == '#') continue; // Remove #

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
        struct Node *person = malloc(sizeof(struct Node));
        for (int i = 0; i < 16; i++) person->children[i] = NULL;
        person->weight = 0;

        if (strcmp(label, "VARDS") == 0) 
        {
            strcpy(person->name, data);
            pob = person;

            for (int i = 0; i <= count_of_people; i++)
            {
                if (people[i] == NULL) break;

                if (strcmp(people[i]->name, data) == 0)
                {
                    pob = people[i];
                    person = people[i];
                    newPerson = 0;
                    break;
                }
            }
            
            incrementAdded = 0;
        }

        if (strcmp(label, "TEVS") == 0)
        {
            strcpy(person->name, data);

            for (int i = 0; i <= count_of_people; i++)
            {
                if (people[i] == NULL) break;

                if (strcmp(people[i]->name, data) == 0)
                {
                    person = people[i];
                    parentIsKnown = 1;
                    break;
                }
            }

            person->weight++;

            // Adding children to this person sequentially.
            for (int i = 0; i <= 16; i++)
            {
                if (person->children[i] == NULL)
                {
                    person->children[i] = pob;
                    break;
                }
            }

            // If this person's children have children than inrement the
            // "level of generation" plus one for outputting family tree.
            if (pob->children[0] != NULL) person->weight++;

            pob->father = person;
        }

        if (strcmp(label, "MATE") == 0)
        {
            strcpy(person->name, data);

            for (int i = 0; i <= count_of_people; i++)
            {
                if (people[i] == NULL) break;

                if (strcmp(people[i]->name, data) == 0)
                {
                    person = people[i];
                    parentIsKnown = 1;
                    break;
                }
            }

            person->weight++;

            // Adding children to this person sequentially.
            for (int i = 0; i <= 16; i++)
            {
                if (person->children[i] == NULL)
                {
                    person->children[i] = pob;
                    break;
                }
            }

            // If this person's children have children than increment the
            // "level of generation" plus one for outputting family tree.
            if (pob->children[0] != NULL) person->weight++;

            pob->mother = person;
        }

        // Adding person to the people array.
        for (int i = 0; i <= count_of_people; i++)
        {
            if (people[i] == NULL) 
            {
                people[i] = person;
                count_of_people++;
                break;
            }

            if (strcmp(people[i]->name, person->name) == 0) break;
        }

         // If current person is new AND his parent is known in people array
        // then do NOT increment "level of generation" for them.
        // ??? Otherwise they might be from a different familty tree.
        if (newPerson == 1 && parentIsKnown == 1)
        {
            // printf("%d::%d", newPerson, parentIsKnown);
            for (int i = 0; i < count_of_people; i++)
            {
                printf("before %s:%d\n", people[i]->name, people[i]->weight);
                if (strcmp(people[i]->name, pob->name) == 0
                   || strcmp(people[i]->name, person->name) == 0)
                {
                    continue;
                }

                // people[i]->weight++;
                people[i]->weight = people[i]->children[0]->weight + 1;
                incrementAdded = 1; // for sake of NOT incrementing twice.
                if (people[i]->weight > global_weight) global_weight = people[i]->weight;
                printf("after %s:%d\n", people[i]->name, people[i]->weight);
            }
        }

        if (person->weight > global_weight) global_weight = person->weight;

        newPerson = 1;
        parentIsKnown = 0;
    }
    
    printf("\n");
    // --- Printing out the family tree (sequence) ---
    while(global_weight >= 0) 
    {
        for(int i = 0; i < count_of_people; i++)
        {
            if (people[i] == NULL) break;
            
            if (people[i]->weight == global_weight)
            {
                printf("(%s:%d)", people[i]->name, people[i]->weight);
            }
        }

        global_weight--;
    }

    printf("\n");
    return 0;
}