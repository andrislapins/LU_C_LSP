#ifndef _COMMON_H_
#define _COMMON_H_

#include <ncurses.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define NAME_LEN 32

char *bin2hex(const unsigned char *input, size_t len);
void err_n_die(const char *fmt, ...);
void str_overwrite_stdout();
void str_trim_lf(char *arr, int length);

#endif