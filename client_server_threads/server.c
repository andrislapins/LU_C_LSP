#include "common.h"
#include <strings.h>
#include <pthread.h>

int main(int argc, char **argv)
{
    int listenfd, connfd, n;
    struct sockaddr_in servaddr;
    uint8_t buff[MAXLINE+1];
    uint8_t recvline[MAXLINE+1];

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        err_n_die("Socket error");
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(SERVER_PORT);

    if ((bind(listenfd, (SA *) &servaddr, sizeof(servaddr))) < 0) {
        err_n_die("Bind error");
    }

    if ((listen(listenfd, SERVER_BACKLOG)) < 0) {
        err_n_die("Listen error");
    }

    for (;;) {
        struct sockaddr_in addr;
        socklen_t addr_len;
        char client_address[MAXLINE+1];

        // Accept blocks until an incoming connection arrives.
        // It returns a "file descriptor" to the connection.
        printf("Waiting for a connection on port %d\n", SERVER_PORT);
        fflush(stdout);
        connfd = accept(listenfd, (SA * ) &addr, &addr_len);

        // Get the IP of the client.
        inet_ntop(AF_INET, &addr, client_address, MAXLINE);
        printf("Client connection: %s\n", client_address);

        // Zero out the receive buffer to make sure it ends up null terminated.
        memset(recvline, 0, MAXLINE);
        // Now read the client's message.
        while( (n = read(connfd, recvline, MAXLINE-1)) > 0) {
            fprintf(stdout, "\n%s\n\n%s", bin2hex(recvline, n), recvline);

            // Hacky way to detect the end of the message.
            if (recvline[n-1] == '\n') {
                break;
            }

            memset(recvline, 0, MAXLINE);
        }

        if (n < 0) {
            err_n_die("Read error");
        }

        // Now send a response.
        snprintf((char*)buff, sizeof(buff), "HTTP/1.0 200 OK\r\n\r\nHello\n\n");

        write(connfd, (char*)buff, strlen((char *)buff));
        close(connfd);
    }
}