#include "common.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>

// #define SERVER_PORT 80

#define MAXLINE 4096
#define SA struct sockaddr

void err_n_die(const char *fmt, ...);

int main(int argc, char **argv)
{
    int sockfd, n;
    int sendbytes;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE];
    char recvline[MAXLINE];

    if (argc != 2) {
        err_n_die("Usage: %s <server address", argv[0]);
    }

    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        err_n_die("Error while creating the socket!");
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);

    // Converts IP addresses from text to binary form.
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
        err_n_die("inet_pton error for %s ", argv[1]);
    }

    if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0) {
        err_n_die("Connect failed!");
    }

    // We're connected. Prepare the message.
    sprintf(sendline, "GET / HTTP/1.1\r\n\r\n");
    sendbytes = strlen(sendline);

    // Send the request == making sure you send it all.
    if (write(sockfd, sendline, sendbytes) != sendbytes) {
        err_n_die("Write error");
    }

    memset(recvline, 0, MAXLINE);
    // Now read the server's response.
    while ( (n = read(sockfd, recvline, MAXLINE-1)) > 0) {
        printf("%s", recvline);
    }

    if (n < 0) {
        err_n_die("Read error");
    }

    exit(0);
}