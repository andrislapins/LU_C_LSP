#include "common.h"

int main(int argc, char **argv)
{
    int server_socket, client_socket;
    int res;
    struct sockaddr_in server_address;

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        err_n_die("Socket error");
    }

    bzero(&server_address, sizeof(server_address));
    server_address.sin_family       = AF_INET;
    server_address.sin_addr.s_addr  = htonl(INADDR_ANY);
    server_address.sin_port         = htons(SERVER_PORT);

    res = bind(server_socket, (SA*)&server_address, sizeof(server_address));
    if (res < 0) {
        err_n_die("Bind error");
    }

    res = listen(server_socket, SERVER_BACKLOG);
    if (res < 0) {
        err_n_die("Listen error");
    }

    for (;;) {
        printf("Waiting for a connection on port %d\n", SERVER_PORT);
        fflush(stdout);
        client_socket = accept(server_socket, (SA*)&server_address, MAXLINE);
        // to be continued...
    }
}