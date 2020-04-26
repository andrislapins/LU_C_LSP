/**
 * 
 * 
 * 
 */

#include "player.h"

volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[NAME_LEN];

char *bin2hex(const unsigned char *input, size_t len) 
{
    char *result;
    char *hexits = "0123456789ABCDEF";

    if (input == NULL || len <= 0) {
        return NULL;
    }

    // (2 hexits+space)/chr + NULL
    int resultlength = (len*3)+1;

    result = malloc(resultlength);
    bzero(result, resultlength);

    for (int i = 0; i < len; i++) {
        result[i*3] =       hexits[input[i] >> 4];
        result[(i*3)+1] =   hexits[input[i] & 0x0F];
        result[(i*3)+2] = ' '; // for readability.
    }

    return result;
}

void str_overwrite_stdout() 
{
    printf("\r%s", "> ");
    fflush(stdout);
}

void str_trim_lf(char *arr, int length)
{
    for (int i = 0; i < length; i++) {
        if (arr[i] == '\n') {
            arr[i] = '\0';
            break;
        }
    }
}

void catch_ctrl_c_and_exit(int sig)
{
    flag = 1;
}

void recv_msg_handler()
{
    char message[BUFFER_SIZE] = {};
    while (1) {
        int receive = recv(sockfd, message, BUFFER_SIZE, 0);

        if (receive > 0) {
            printf("%s ", message);
            str_overwrite_stdout();
        } else if (receive == 0) {
            break;
        }

        bzero(message, BUFFER_SIZE);
    }
}

void send_msg_handler()
{
    char buffer[BUFFER_SIZE] = {};
    char message[BUFFER_SIZE + NAME_LEN] = {};

    while (1) {
        str_overwrite_stdout();
        fgets(buffer, BUFFER_SIZE, stdin);
        str_trim_lf(message, BUFFER_SIZE);

        // printf("HEX: %s\n", bin2hex(buffer, BUFFER_SIZE));
        if (strncmp(buffer, "exit", 4) == 0) {
            break;
        } else {
            sprintf(message, "(%s): %s", name, buffer);
            send(sockfd, message, strlen(message), 0);
        }

        bzero(buffer, BUFFER_SIZE);
        bzero(message, BUFFER_SIZE + NAME_LEN);
    }

    catch_ctrl_c_and_exit(2);
}

int main(int argc, char **argv)
{
    if(argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *ip = "127.0.0.1";
    int port = atoi(argv[1]);

    signal(SIGINT, catch_ctrl_c_and_exit);

    printf("Enter your name: ");
    fgets(name, NAME_LEN, stdin);
    str_trim_lf(name, strlen(name));

    if (strlen(name) > NAME_LEN - 1 || strlen(name) < 2) {
        printf("Enter name correctly\n");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;

    // Socket settings.
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family        = AF_INET;
    server_addr.sin_addr.s_addr   = inet_addr(ip);
    server_addr.sin_port          = htons(port);

    // Connect to the server.
    int err = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (err == -1) {
        printf("ERROR: connect\n");
        return EXIT_FAILURE;
    }

    // Send the name.
    send(sockfd, name, NAME_LEN, 0);
    printf("=== WELCOME to the chatroom ===\n");

    pthread_t send_msg_thread;
    if (pthread_create(&send_msg_thread, NULL, (void*)send_msg_handler, NULL) != 0) {
        printf("ERROR: pthread send\n");
        return EXIT_FAILURE;
    }

    pthread_t recv_msg_thread;
    if (pthread_create(&recv_msg_thread, NULL, (void*)recv_msg_handler, NULL) != 0) {
        printf("ERROR: pthread recv\n");
        return EXIT_FAILURE;
    }

    while(1) {
        if (flag) {
            printf("\nBye\n");
            break;
        }
    }

    close(sockfd);
    return EXIT_SUCCESS;
}