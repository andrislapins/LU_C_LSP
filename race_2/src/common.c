#include "../include/common.h"
#include "../include/protocol.h"
#include "../include/serialization.h"

char ip[IP_LEN];

void err_die(char* err_msg) {
    printf("ERROR: %s\n", err_msg);
    printf("(errno = %d): %s\n", errno, strerror(errno));

    exit(EXIT_FAILURE);
}

void print_array_in_hex(char *name, void *arr, int len) {
    char *arr_p = arr;

    printf("(%s with %d bytes): ", name, len);
    for (int i = 0; i < len; arr_p++, i++) {
        printf("%02x ", *arr_p);
    }
    printf("\n");
}

void generate_password(client_t *client) {
    srand((unsigned int)time(0));
    int i, r;
    char a[66];

    for (i = 0; i < 26; i++) {
        a[i] = 'a' + i;
        a[i+26] = 'A' + i;
    }

    for (i = 0; i < 10; i++) {
        a[i+52] = '0' + i;
    }

    a[62] = '!';
    a[63] = '+';
    a[64] = '#';
    a[65] = '&';

    for (i = 0; i < PLAYER_PASS_LEN-1; i++) {
        r = rand() % 65;
        client->player_pass[i] = a[r];
    }
    client->player_pass[i] = '\0';

    printf("Player %s obtained password - %s\n", 
        client->player_name, client->player_pass);
}

char *ip_addr(struct sockaddr_in addr) {
    memset(ip, 0, IP_LEN);
    snprintf(ip, IP_LEN, "%d.%d.%d.%d",
        addr.sin_addr.s_addr & 0xff,
        (addr.sin_addr.s_addr & 0xff00) >> 8,
        (addr.sin_addr.s_addr & 0xff0000) >> 16,
        (addr.sin_addr.s_addr & 0xff000000) >> 24);

    return ip;
}

void get_group_n_id(int *base, int *group, int *index) {
    *group = *base / 100;
    *index = *base % 100;
}