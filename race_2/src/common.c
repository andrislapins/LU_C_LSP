#include "../include/common.h"
#include "../include/protocol.h"
#include "../include/serialization.h"
#include "../include/log_messages.h"

// For IP addressing deserialisation.
char ip[IP_LEN];
// For not knowing the client's name.
char na[] = "N/A";
// For formatting client's messages.
char msg_buf[MSG_BUF_LEN];

void print_array_in_hex(char *name, void *arr, int len) {
    char *arr_p = arr;

    printf("(%s with %d bytes): ", name, len);
    for (int i = 0; i < len; arr_p++, i++) {
        printf("%02x ", *arr_p);
    }
    printf("\n");
}

void generate_password(FILE *fp, client_t *client) {
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

    for (i = 0; i < CLIENT_PASS_LEN-1; i++) {
        r = rand() % 65;
        client->password[i] = a[r];
    }

    client->password[CLIENT_PASS_LEN-1] = '\0';

    print_array_in_hex("in func", client->password, CLIENT_PASS_LEN);

    log_pass_generated(fp, client);
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

char *from_who(client_t *client) {
    if (strlen(client->player->name) != 0) {
        return client->player->name;
    }

    return na;
}

void help() {
    printf(
        "%sUsage format:\n",
        ANSI_HACK
    );
    printf("executable [-a ip-address] [-p port-number] [-l output-file] [-h]\n");
    printf(
        "Default values:\n-a=127.0.0.1, -p=%d, -l=stdout%s\n",
        PORT, ANSI_RESET_ALL
    );
}