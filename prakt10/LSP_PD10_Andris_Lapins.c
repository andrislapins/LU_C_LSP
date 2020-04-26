/**
 *  Author: Andris Lapiņš, al18011 
 *  Description: Utilize socketpair() function to send message between parent
 *  process and child process.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void child(int socket)
{
    // Buffer with enough size to hold the received message AND the sum of 
    // those numbers as a string.
    char buf[8];
    int nr_1, nr_2, sum;

    // Read from parent socket.
    read(socket, buf, sizeof(buf));
    sscanf(buf, "%d %d", &nr_1, &nr_2);
    printf("Child received numbers - %d and %d\n", nr_1, nr_2);

    // Calculate and send the sum of the given numbers to parent process.
    sum = nr_1 + nr_2;
    sprintf(buf, "%d", sum);
    write(socket, buf, sizeof(buf));
}

void parent(int socket)
{
    char buf[8];
    char *numbers = "74 33";
    // Send the numbers to child.
    write(socket, numbers, sizeof(numbers));

    // Receive the calculated sum of those numbers from child.
    int n = read(socket, buf, sizeof(buf));
    printf("Parent received the sum of those numbers - %.*s\n", n, buf);
}

int main(void)
{
    int fd[2];
    int connection;
    static const int parent_socket = 0;
    static const int childsocket = 1;
    pid_t pid;

    connection = socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
    if (connection == -1) {
        perror("Socket connection failed");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == 0) {
        close(fd[parent_socket]);
        child(fd[childsocket]);
    } else {
        close(fd[childsocket]);
        parent(fd[parent_socket]);
    }

    exit(EXIT_SUCCESS);
}