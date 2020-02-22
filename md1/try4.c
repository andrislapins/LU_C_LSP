/**
 * Author: Andris Lapins, al18011
 * Description:
 * Last time modified/compiled: 20.02.2020
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int manage_people(void);
int print_families(void);

struct Person{
    char name[57];
    int weight; // Store the level of generation;
    int temp_weight; // Store weight of "not already declared" person.
    int checked; // Store the value if the block of family is traversed.
    struct Person* father;
    struct Person* mother;
    struct Person* children[8];
    struct Person* partners[8];
};

// Created type Array to more easily manage the dynamic array of people.
typedef struct{
    int arr_size;
    struct Person** arr; // Array of pointers.
} Array;

Array people;
struct Person* person; // A temporary pointer to store the current person of the
                       // line which was read.
struct Person* child; // A temporary pointer to store the child of the format.
struct Person* partner; // A temporary pointer to store the first partner of the
                        // family format.

char line[64];
char label[6];
char data[57];

int population_count = 0; // To store the count of registered people.
int global_weight = 0; //  The highest number of level of generation.
int global_weight_min = 0; // The lowest number of level of generation. // UNUSED
int partner_counter = 2; // This counts the number of TEVS and MATE label
                         // occurences.

// These values are for checking the right number of occurences of these labels.
int wasVARDS = 0;
int wasTEVS = 0;
int wasMATE = 0;

int main(void)
{
    int result;
    people.arr_size = 1024; // Initial people array size.

    people.arr = malloc(sizeof(struct Person*)*people.arr_size);
    person = malloc(sizeof(struct Person));
    child = malloc(sizeof(struct Person));
    partner = malloc(sizeof(struct Person));

    result = manage_people();
    if (result == 1) 
    {
        return 1;
    }

    result = print_families();
    if (result == 1) 
    {
        return 1;
    }

    printf("\n");
    return 0;
}

int manage_people()
{
    int r;
    while (fgets(line, 64, stdin))
    {
        // Error checking. // Line starting with # are comments in the file.
        if (line[0] == '\n' || line[0] == '#') continue;

        r = sscanf(line, "%s %s", label, data);
        if (r == EOF)
        {
            printf("Could not scan a line in your data\n");
            return 1;
        }

        // Detecting multiple occurences within single family format.
        if (wasVARDS > 1)
        {
            printf("The label VARDS occurs in one format multiple times\n");
            return 1;
        }

        if (wasTEVS > 1)
        {
            printf("The label TEVS occurs in one format multiple times\n");
            return 1;
        }

        if (wasMATE > 1)
        {
            printf("The label MATE occurs in one format multiple times\n");
            return 1;
        }

        if (wasVARDS == 1 && wasTEVS == 1 && wasMATE == 1)
        {
            wasVARDS = 0;
            wasTEVS = 0;
            wasMATE = 0;
        }

        // Initializing person.
        struct Person* person = malloc(sizeof(struct Person));
        person->father = NULL;
        person->mother = NULL;
        person->weight = 0;
        person->temp_weight = 0;
        person->checked = 0;

        for (int i = 0; i < 8; i++)
        {
            person->children[i] = NULL;
        
        }
        for (int i = 0; i < 8; i++)
        {
            person->partners[i] = NULL;
        }

        // Arranging the person in people array OR finding existent one.
        for (int i = 0; i <= population_count; i++)
        {
            if (people.arr[i] == NULL)
            {
                people.arr[i] = malloc(sizeof(struct Person));
                people.arr[i] = person;
                population_count++;
                break;
            }

            if (strcmp(data, people.arr[i]->name) == 0)
            {
                person = people.arr[i];
                break;
            }
        }

        // Detecting the label of a person.
        if (strcmp(label, "VARDS") == 0)
        {
            strcpy(person->name, data);
            child = person;

            partner = NULL;
            wasVARDS++;
        }

        // Detecting label TEVS.
        if (strcmp(label, "TEVS") == 0)
        {
            // Detecting cycles of family members.
            if (person->father != NULL)
            {
                if (strcmp(child->name, person->father->name) == 0
                    || strcmp(child->name, person->mother->name) == 0)
                {
                    printf("A cycle detected between ");
                    printf("%s and %s\n", child->name, person->name);
                    return 1;
                }
            }

            // Detecting multiple parents for a single person.
            if (child->mother != NULL)
            {
                if (strcmp(child->mother->name, "null") != 0
                    && child->checked == 1)
                {
                    printf("A person %s can not have two ", child->name);
                    printf("different mothers\n");
                    return 1;
                }
            }
            
            if (child->father != NULL)
            {
                if (strcmp(child->father->name, "null") != 0
                    && child->checked == 1)
                {
                    printf("imin");
                    printf("A person %s can not have two ", child->name);
                    printf("different fathers\n");
                    return 1;
                }
            }

            // Registering person as a father.
            strcpy(person->name, data);

            child->father = malloc(sizeof(struct Person));
            child->father = person;

            person->weight = child->weight + 1 + child->temp_weight;
            child->temp_weight = 0;

            // Initialize father if the person did not acquire father before.
            // Otherwise, increment this person's father's weight.
            if (person->father == NULL)
            {
                person->father = malloc(sizeof(struct Person));
                strcpy(person->father->name, "null");
                person->father->weight = 0;
                person->father->temp_weight = child->weight + 1;
                person->father->father = NULL;
                person->father->mother = NULL;

                for (int i = 0; i < 8; i++)
                {
                    person->father->children[i] = NULL;
                }
            } else {
                person->father->weight++;
            }

            // Initialize mother if the person did not acquire mother before.
            // Otherwise, increment this person's mother's weight.
            if (person->mother == NULL)
            {
                person->mother = malloc(sizeof(struct Person));
                strcpy(person->mother->name, "null");
                person->mother->weight = 0;
                person->mother->temp_weight = child->weight + 1;
                person->mother->father = NULL;
                person->mother->mother = NULL;

                for (int i = 0; i < 8; i++)
                {
                    person->mother->children[i] = NULL;
                }
            } else {
                person->mother->weight++;
            }

            // Adding a partner to the person's partner list.
            if (partner == NULL) 
            {
                partner = person;
            } else {
                for (int i = 0; i < 8; i++)
                {
                    if (person->partners[i] == NULL)
                    {
                        person->partners[i] = malloc(sizeof(struct Person));
                        person->partners[i] = partner;
                        break;
                    }
                }
            }

            wasTEVS++;
        }

        // Detecting label MATE.
        if (strcmp(label, "MATE") == 0)
        {
            // Detecting cycles of family members.
            if (person->mother != NULL)
            {
                if (strcmp(child->name, person->mother->name) == 0
                    || strcmp(child->name, person->father->name) == 0)
                {
                    printf("A cycle detected between ");
                    printf("%s and %s\n", child->name, person->name);
                    return 1;
                }
            }

            // Detecting multiple parents for a single person.
            if (child->mother != NULL)
            {
                if (strcmp(child->mother->name, "null") != 0
                    && child->checked == 1)
                {
                    printf("A person %s can not have two ", child->name);
                    printf("different mothers\n");
                    return 1;
                }
            }
            
            if (child->father != NULL)
            {
                if (strcmp(child->father->name, "null") != 0 
                    && child->checked == 1)
                {
                    printf("%s", child->name);
                    printf("A person %s can not have two ", child->name);
                    printf("different fathers\n");
                    return 1;
                }
            }

            // Registering person as a mother.
            strcpy(person->name, data);

            child->mother = malloc(sizeof(struct Person));
            child->mother = person;

            person->weight = child->weight + 1 + child->temp_weight;
            child->temp_weight = 0;

            // Initialize father if the person did not acquire father before.
            // Otherwise, increment this person's father's weight.
            if (person->father == NULL)
            {
                person->father = malloc(sizeof(struct Person));
                strcpy(person->father->name, "null");
                person->father->weight = 0;
                person->father->temp_weight = child->weight + 1;
                person->father->father = NULL;
                person->father->mother = NULL;

                for (int i = 0; i < 8; i++)
                {
                    person->father->children[i] = NULL;
                }
            } else {
                person->father->weight++;
            }

            // Initialize mother if the person did not acquire mother before.
            // Otherwise, increment this person's mother's weight.
            if (person->mother == NULL)
            {
                person->mother = malloc(sizeof(struct Person));
                strcpy(person->mother->name, "null");
                person->mother->weight = 0;
                person->mother->temp_weight = child->weight + 1;
                person->mother->father = NULL;
                person->mother->mother = NULL;

                for (int i = 0; i < 8; i++)
                {
                    person->mother->children[i] = NULL;
                }
            } else {
                person->mother->weight++;
            }

            // Adding a partner to the person's partner list.
            if (partner == NULL) 
            {
                partner = person;
            } else {
                for (int i = 0; i < 8; i++)
                {
                    if (person->partners[i] == NULL)
                    {
                        person->partners[i] = malloc(sizeof(struct Person));
                        person->partners[i] = partner;
                        break;
                    }
                }
            }

            // For the purposes of checking that this child(person) was
            // already included as a child in other family member format.
            child->checked = 1;

            wasMATE++;
        }

        // Adding children in sequence for the person.
        for (int i = 0; i < 8; i++)
        {
            if (person->children[i] == NULL)
            {
                person->children[i] = malloc(sizeof(struct Person));
                person->children[i] = child;
                break;
            }
        }

        // Checking the global weight or "the level of generation".
        if (person->weight > global_weight)
        {
            global_weight = person->weight;
        }

        // Checking if there is a need for expanding people array.
        if (population_count == people.arr_size - 1)
        {
            people.arr = realloc(people.arr, 2*population_count);
            people.arr_size = 2*population_count;

            if (people.arr == NULL)
            {
                printf("Could not allocate memory for people\n");
                return 1;
            }
        }
    } 
    // End of while loop;

    // Balancing the weights between children and parents.
    for (int i = 0; i < population_count; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (people.arr[i]->children[j] == NULL) break;

            if (people.arr[i]->weight <= people.arr[i]->children[j]->weight)
            {
                int w = people.arr[i]->children[j]->weight + 1;
                people.arr[i]->weight = w;
            }

            // Checking the global weight or "the level of generation".
            if (people.arr[i]->weight > global_weight)
            {
                global_weight = people.arr[i]->weight;
            }
        }
    }

    // Balancing the weights between partners.
    for (int i = 0; i < population_count; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (people.arr[i]->partners[j] == NULL) break;

            // TODO: Would want to optimise this for cases when the right order
            // checking partners would not be so lucky like in my test cases.
            if (people.arr[i]->partners[j]->weight != people.arr[i]->weight)
            {
                people.arr[i]->partners[j]->weight = people.arr[i]->weight;
            }
        }
    }

    return 0;
}

int print_families()
{
    while (global_weight >= global_weight_min)
    {
        for (int i = 0; i < population_count; i++)
        {
            // Printing out people in order of their level of generation.
            if (people.arr[i]->weight == global_weight)
            {
                // My output structure to follow the levels of generation.
                // printf("(%s:%d)\n", people.arr[i]->name, people.arr[i]->weight);
                printf("%s\n", people.arr[i]->name, people.arr[i]->weight);
            }
        }

        global_weight--;
    }

    return 0;
}