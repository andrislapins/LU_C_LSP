#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h> // size_t, NULL, EXIT_SUCCESS, atoi(), malloc etc.
#include <string.h> // memcpy(), strcmp() etc.
#include <strings.h> // bzero() etc.
#include <unistd.h> // _POSIX_VERSION etc.
#include <errno.h> // symbol constants in case of error.
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <getopt.h>
#include <sys/types.h> // pthread_mutex_t, pthread_cond_t etc.
#include <sys/socket.h>
#include <netinet/in.h> // struct in_addr, sin_family, AF_INET etc.
#include <arpa/inet.h> // inet_addr(), htonl() etc.

#include "protocol.h"

#define IP_LEN 17
#define PORT 9999
#define MAX_CLIENTS 4

#define MAX_BUFFER_SIZE 256
#define DIGITS_LEN 5 // To (de)serilaize digits of length 4.
#define COUNT_OF_FIELDS 1 // is this needed ?

#define MSG_TYPE_LEN 3
#define ERR_MSG_LEN 48
#define CLIENT_NAME_LEN 30
#define CLIENT_PASS_LEN 10
#define GAME_NAME_LEN 20

#define FIELD_NAME_LEN 20

// Terminal output format definitions.
// EXP: [x; (0-regular, 1-bold): \e == \033
#define ANSI_RESET_ALL  "\e[0m"
#define ANSI_BOLD       "\e[1m"
#define ANSI_BLINK      "\e[5m"

#define ANSI_RED "\e[31m"
#define ANSI_GREEN "\e[32m"

// NOTE:? Could I typdef(verb) all protocol structs and safely use/manage them.

// Composite data structures of the race game.
typedef struct track {
    struct Field *field;
    struct Line *start_line, *main_line;
    int n_extra_lines;
} track_t;

typedef struct game{
    int ID;
    struct Game *game_h;
    track_t *track;
} game_t;

typedef struct client {
    struct Player_info *player;
    char *password, *ip;
    game_t *game;

    int sock_fd;
    struct sockaddr_in address;
} client_t;

void print_array_in_hex(char *name, void* arr, int len);
void generate_password(client_t *client);
char *ip_addr(struct sockaddr_in addr);
void get_group_n_id(int *base, int *group, int *index);
char *from_who(client_t *client);

#endif