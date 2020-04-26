#include "common.h"
#include "player.h"
#include "serialization.h"

int sockfd = 0;
char name[NAME_LEN];
Player_t *p;
volatile sig_atomic_t flag = 0;

pthread_mutex_t player_mutex = PTHREAD_MUTEX_INITIALIZER;

void catch_ctrl_c_and_exit(int sig) {
    flag = 1;
}

void recv_msg_handler() {
    char message[BUFFER_SIZE];
    bzero(message, BUFFER_SIZE);

    while (1) {
        int receive = recv(sockfd, message, BUFFER_SIZE, 0);
        pthread_mutex_lock(&player_mutex);

        deserialize_player(message, p);
        display(p);
        wrefresh(p->curwin);

        pthread_mutex_unlock(&player_mutex);
        bzero(message, BUFFER_SIZE);
    }
}

void send_msg_handler() {
    char message[BUFFER_SIZE];
    int success;

    bzero(message, BUFFER_SIZE);

    while (1) {
        pthread_mutex_lock(&player_mutex);

        success = send_player(sockfd, p);
        if (!success) {
            err_n_die("ERROR: send_player\n");
        }

        pthread_mutex_unlock(&player_mutex);
        bzero(message, BUFFER_SIZE);
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
    printf("=== WELCOME to the chatroom ===\n"); // Get this msg from server.

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


    // Ncurses initialization.
    initscr();
    noecho();
    cbreak();
    curs_set(FALSE);

    int scr_y_max, scr_x_max;
    getmaxyx(stdscr, scr_y_max, scr_x_max);

    WINDOW *play_win = newwin(20, scr_x_max-20, (scr_y_max/2)-10, 10);
    box(play_win, 0, 0);
    mvwprintw(stdscr, 24, (scr_x_max/2)-8, "Press x to quit");
    refresh();
    wrefresh(play_win);
    keypad(play_win, TRUE);

    p = init_player(play_win, 1, 1, '@');

    do {
        printf("IN HERE do while\n");
        refresh();
    } while(getmv(p) != 'x' || flag == 1);
    
    // Garbage....
    clear();
    refresh(); // ---
    sleep(1);
    move(2, 2);
    printf("\nBye\n");
    refresh(); // ---
    sleep(2);
    // Garbage ...

    endwin();
    close(sockfd);
    return EXIT_SUCCESS;
}