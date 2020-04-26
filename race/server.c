#include "player.h"

static _Atomic unsigned int cli_count = 0;
static int uid = 10;

// Client structure.
typedef struct {
    struct sockaddr_in address;
    int sockfd;
    int uid;
    char name[NAME_LEN];
} client_t;

// Store pointers of client fds.
client_t *clients[MAX_CLIENTS];

// Mutex initilizer.
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void queue_add(client_t *cl) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i]) {
            clients[i] = cl;
            break;
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

void queue_remove(int uid) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i]) {
            if(clients[i]->uid == uid) {
                clients[i] = NULL;
                break;
            }
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

void print_ip_addr(struct sockaddr_in addr) {
    printf("%d.%d.%d.%d",
        addr.sin_addr.s_addr & 0xff,
        (addr.sin_addr.s_addr & 0xff00) >> 8,
        (addr.sin_addr.s_addr & 0xff0000) >> 16,
        (addr.sin_addr.s_addr & 0xff000000) >> 24);
}

void send_message(char *s, int uid) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i]) {
            if(clients[i]->uid != uid) {
                if (write(clients[i]->sockfd, s, strlen(s)) < 0) {
                    printf("ERROR: write to descriptor failed\n");
                    break;
                }
            }
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

void *handle_client(void *arg) {
    char buffer[BUFFER_SIZE];
    char name[NAME_LEN];
    int leave_flag = 0;
    cli_count++;

    client_t *cli = (client_t*)arg;

    // Name.
    if (recv(cli->sockfd, name, NAME_LEN, 0) <= 0 || strlen(name) < 2 || strlen(name) >= NAME_LEN - 1) {
        printf("Enter the name correctly\n");
        leave_flag = 1;
    } else {
        strcpy(cli->name, name);
        sprintf(buffer, "%s has joined\n", cli->name);
        printf("%s", buffer);
        send_message(buffer, cli->uid);
    }

    bzero(buffer, BUFFER_SIZE);

    while (1) {
        if (leave_flag) {
            break;
        }

        int receive = recv(cli->sockfd, buffer, BUFFER_SIZE, 0);
        if (receive > 0) {
            if (strlen(buffer) > 0) {
                printf("%s", buffer);
                send_message(buffer, cli->uid);
            }
        } else if (receive == 0 || strcmp(buffer, "exit") == 0) {
            sprintf(buffer, "%s has left\n", cli->name);
            printf("%s", buffer);
            send_message(buffer, cli->uid);
            leave_flag = 1;
        } else {
            printf("ERROR: -1\n");
            leave_flag = 1;
        }

        bzero(buffer, BUFFER_SIZE);
    }

    close(cli->sockfd);
    queue_remove(cli->uid);
    free(cli);
    cli_count--;
    pthread_detach(pthread_self());

    return NULL;
}

int main(int argc, char **argv) 
{
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *ip = "127.0.0.1";
    int port = atoi(argv[1]);

    int option = 1;
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    pthread_t tid;

    // Socket settings.
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family        = AF_INET;
    serv_addr.sin_addr.s_addr   = inet_addr(ip);
    serv_addr.sin_port          = htons(port);

    // Signals.
    signal(SIGPIPE, SIG_IGN);

    if (setsockopt(listenfd, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char*)&option, sizeof(option)) < 0) {
        printf("ERROR: setsockopt\n");
        return EXIT_FAILURE;
    }

    if (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("ERROR: bind\n");
        return EXIT_FAILURE;
    }

    if (listen(listenfd, MAX_CLIENTS) < 0) {
        printf("ERROR: listen\n");
        return EXIT_FAILURE;
    }

    printf("=== WELCOME to the chatroom ===\n");
    
    while(1) {
        socklen_t clilen = sizeof(cli_addr);
        connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &clilen);

        if ((cli_count + 1) == MAX_CLIENTS) {
            printf("Maximum clients connected.\n");
            print_ip_addr(cli_addr);
            close(connfd);
            continue;
        }

        // Client settings.
        client_t *cli = (client_t *)malloc(sizeof(client_t));
        cli->address    = cli_addr;
        cli->sockfd     = connfd;
        cli->uid        = uid++;

        // Add client to queue.
        queue_add(cli);
        pthread_create(&tid, NULL, &handle_client, (void*)cli);

        // Reduce CPU usage.
        sleep(1);
    }

    return EXIT_SUCCESS;
}