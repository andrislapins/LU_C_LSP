#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <limits.h>
#include <sys/select.h>

#define SERVER_PORT 8989
#define BUFFER_SIZE 4096
#define SOCKET_ERROR (-1)
#define SERVER_BACKLOG 100

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

void * handle_connection(int);
int check(int exp, const char *msg);
int accept_new_connection(int server_socket);
int setup_server(short port, int backlog);

int main(int argc, char **argv) {
    int server_socket = setup_server(SERVER_PORT, SERVER_BACKLOG);
    fd_set current_sockets, ready_sockets;

    int max_socket_so_far = 0;

    // Initialize my current set.
    FD_ZERO(&current_sockets);
    FD_SET(server_socket, &current_sockets);

    printf("FD_SETSIZE=%d\n", FD_SETSIZE);

    while(true) {
        // Because select is destructive.
        ready_sockets = current_sockets;

        if (select(max_socket_so_far, &ready_sockets, NULL, NULL, NULL) < 0) {
            perror("Select error");
            exit(1);
        }

        for (int i = 0; i < max_socket_so_far; i++) {
            if (FD_ISSET(i, &ready_sockets)) {
                if (i == server_socket) {
                    // This is a new connection.
                    int client_socket = accept_new_connection(server_socket);
                    FD_SET(client_socket, &current_sockets);
                    
                    if (client_socket > max_socket_so_far) {
                        max_socket_so_far = client_socket;
                    }
                } else {
                    // Do whatever we do with connections.
                    handle_connection(i);
                    FD_CLR(i, &current_sockets);
                }
            }
        }

        // printf("Waiting for connections...\n");
        // // Wait for, and eventually accept an incoming connection.
        // int client_socket = accept_new_connection(server_socket);

        // // Do whatever we do with connections.
        // handle_connection(client_socket);
    }

    return EXIT_SUCCESS;
}

int setup_server(short port, int backlog) {
    int server_socket, client_socket, addr_size;
    SA_IN server_addr;

    check((server_socket = socket(AF_INET, SOCK_STREAM, 0)), 
            "Failed to create socket");

    // Initialize the address struct.
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    check(bind(server_socket, (SA*)&server_addr, sizeof(server_addr)),
            "Bind failed");
    check(listen(server_socket, backlog),
            "Listen failed");
        
    return server_socket;
}

int accept_new_connection(int server_socket) {
    int addr_size = sizeof(SA_IN);
    int client_socket;
    SA_IN client_addr;

    check(client_socket = 
            accept(server_socket,
                    (SA*)&client_addr,
                    (socklen_t*)&addr_size),
                    "Accept failed");

    return client_socket;
}

int check(int exp, const char *msg) {
    if (exp == SOCKET_ERROR) {
        perror(msg);
        exit(1);
    }

    return exp;
}

void * handle_connection(int client_socket) {
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    int msgsize = 0;
    char actual_path[_PC_PATH_MAX+1]; // What is this size (_PC_PATH_MAX) ?

    // Read the client's message -- the name of the file to read.
    while ((bytes_read = read(client_socket, buffer+msgsize, sizeof(buffer)-msgsize-1))) {
        msgsize += bytes_read;
        if (msgsize > BUFFER_SIZE-1 || buffer[msgsize-1] == '\n') {
            break;
        }
    }

    check(bytes_read, "Recv error");
    buffer[msgsize-1] = 0; // Null terminate the message and remove the \n.

    printf("REQUEST: %s\n", buffer);
    fflush(stdout);

    // Validity check.
    if (realpath(buffer, actual_path) == NULL) {
        printf("ERROR(bad path): %s\n", buffer);
        close(client_socket);

        return NULL;
    }

    // Read file and send its contents to client.
    FILE *fp = fopen(actual_path, "r");
    if (fp == NULL) {
        printf("ERROR(open): %s\n", buffer);
        close(client_socket);

        return NULL;
    }

    // Read file contents and send them to client.
    // NOTE: this is a fine example program, but rather insecure.
    // A real program would probably limit the client to certain files.
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0) {
        write(client_socket, buffer, bytes_read);
    }

    close(client_socket);
    fclose(fp);
    printf("Closing connection\n");
    return NULL;
}