#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
// For size_t, NULL, EXIT_SUCCESS, atoi(), malloc etc.
#include <stdlib.h>
#include <string.h>
// For bzero() etc.
#include <strings.h>
// For _POSIX_VERSION etc.
#include <unistd.h>
// For symbol constants in case of error.
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>

// For pthread_mutex_t, pthread_cond_t etc.
#include <sys/types.h>
#include <sys/socket.h>

// For struct in_addr, sin_family, AF_INET etc.
#include <netinet/in.h>
// For inet_addr(), htonl() etc.
#include <arpa/inet.h>

#include "protocol.h"

#define PORT 9999
#define IP_LEN 16
#define MAX_CLIENTS 4
#define MAX_BUFFER_SIZE 257
#define COUNT_OF_FIELDS 1

// These lengths are [useful data]+1 size because of the need to keep last element \0.
#define MSG_TYPE_LEN 3
#define PLAYER_NAME_LEN 31
#define PLAYER_PASS_LEN 11
#define GAME_NAME_LEN 21

#define FIELD_NAME_LEN 21

typedef struct {
    int player_id;
    char player_name[PLAYER_NAME_LEN];
    char player_pass[PLAYER_PASS_LEN];

    char curr_game_name[GAME_NAME_LEN];
    int curr_game_id;
    int chosen_field_id;

    int sock_fd;
    struct sockaddr_in address;
    char ip[IP_LEN];
} client_t;

typedef struct {
    struct Field *field;
    struct Line *start_line;
    struct Line *main_line;
    int n_extra_lines;
} field_t;

// For handling errors and killing the process.
void err_die(char* err_msg);
void print_array_in_hex(char *name, void* arr, int len);
void generate_password(client_t *client);
char *ip_addr(struct sockaddr_in addr);
void get_group_n_id(int *base, int *group, int *index);

#endif