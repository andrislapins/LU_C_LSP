#include <stdio.h>
#include <stdlib.h>

struct Node
{
    void *data;
    struct Node *next;
};


void push(struct Node **head_ref, void *new_data, size_t data_size) {
    struct Node *new_node = (struct Node*)malloc(sizeof(struct Node));

    new_node->data = malloc(data_size);
    new_node->next = (*head_ref);

    int i;
    for (i = 0; i < data_size; i++) {
        *(char*)(new_node->data + i) = *(char*)(new_data + i);
    }

    (*head_ref) = new_node;
}

void print_list(struct Node *node, void (*fptr)(void*)) {
    while (node != NULL) {
        (*fptr)(node->data);
        node = node->next;
    }
    printf("\n");
}

void print_int(void *n) {
    printf(" %d", *(int*)n);
}

void print_float(void *f) {
    printf(" %f", *(float*)f);
}

int main() {
    struct Node *start = NULL;

    unsigned int_size = sizeof(int);
    int arr[] = {10, 20, 30, 40, 50}, i;
    printf("size of arr: %ul\n", sizeof(arr));

    for (i = 4; i >= 0; i--) {
        push(&start, &arr[i], int_size);
    }

    printf("Created integer linked list is: \n");
    print_list(start, print_int);

    start = NULL;

    unsigned float_size = sizeof(float);
    float arr2[] = {10.1, 20.1, 30.1, 40.1, 50.1};
    printf("size of arr: %ul\n", sizeof(arr));

    for (i = 4; i >= 0; i--) {
        push(&start, &arr2[i], float_size);
    }

    printf("Created float linked list is: \n");
    print_list(start, print_float);

    return 0;
}