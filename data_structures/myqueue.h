#ifndef MYQUEUE_H_
#define MYQUEUE_H_

struct node {
    struct Node *next;
    int *client_socket;
};
// typedef struct node node_t;

#define node_t struct node

void enqueue(int *client_socket);
int *dequeue();

#endif