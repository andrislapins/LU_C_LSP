#include "../include/common.h"
#include "../include/protocol.h"
#include "../include/serialization.h"
#include "../include/my_strings.h"
#include <ncurses.h>

void handle_sigint(int sig);

int get_number_of_fields(char* buffer, client_t *myclient, int serv_sock) {
    int n_field_ids, chose;
    ssize_t data_n;
    char msg_type[MSG_TYPE_LEN] = "NF\0";
    // Expecting the entered field id contain 4 digits. +1 is for \0 character.
    int n_digits = 5;
    char *num_str = (char*)calloc(n_digits, 1);

    // Serialize the data.
    bzero(buffer, MAX_BUFFER_SIZE);
    serialize_msg_NF(buffer, msg_type);

    // Initiate sending and receiving.
    data_n = send(serv_sock, buffer, MAX_BUFFER_SIZE, 0);
    if (data_n < 0) {
        free(myclient);
        free(buffer);
        err_die("Could not send message of type NUMBER OF FIELDS!");
    }

    bzero(buffer, MAX_BUFFER_SIZE);
    data_n = recv(serv_sock, buffer, MAX_BUFFER_SIZE, 0);
    if (data_n < 0) {
        free(myclient);
        free(buffer);
        err_die("Could not receive message of type NUMBER OF FIELDS!");
    }

    deserialize_msg_NF_response(buffer, &n_field_ids);

    printf("Choose a field in range from 1 to %d to get more info: \n", n_field_ids);
    fgets(num_str, n_digits, stdin);

    chose = atoi(num_str);
    printf("You chose: %d\n", chose);

    /* FIELD INFO */

    msg_type[MSG_TYPE_LEN] = "FI\0";
    bzero(buffer, MAX_BUFFER_SIZE);
    serialize_msg_FI(buffer, msg_type, chose);

    data_n = send(serv_sock, buffer, MAX_BUFFER_SIZE, 0);
    if (data_n < 0) {
        free(myclient);
        free(buffer);
        err_die("Could not send message of type FIELD INFO!");
    }

    bzero(buffer, MAX_BUFFER_SIZE);
    data_n = recv(serv_sock, buffer, MAX_BUFFER_SIZE, 0);
    if (data_n < 0) {
        free(myclient);
        free(buffer);
        err_die("Could not receive message of type FIELD INFO!");
    }

    deserialize_msg_FI_response(buffer);

    printf("Field ID: %d\n", 0);
    printf("Field name: %s\n", "bestfieldname");
    printf("Field width: %d\n", 0);
    printf("Field height: %d\n", 0);
    printf("Start line x: %d, y: %d\n", 0, 0);
    printf("Main line x: %d, y: %d\n", 0, 0);
    printf("Number of extra lines: %d\n", 1);
    printf("Extra line one x: %d, y: %d\n", 0, 0);

    // later ask for FIELD INFO for a particular field until one is chosen in a while loop.

    return n_field_ids;
}

void create_game(char *buffer, client_t *myclient, int serv_sock, int field_id) {
    ssize_t data_n;

    char msg_type[MSG_TYPE_LEN] = "CG\0";
    char *player_name = (char*)calloc(PLAYER_NAME_LEN, 1);
    char *game_name = (char*)calloc(GAME_NAME_LEN, 1);

    // Set the player's name.
    printf("Your name? (max %d symbols): ", PLAYER_NAME_LEN-1);
    fgets(player_name, PLAYER_NAME_LEN, stdin);

    player_name[strlen(player_name)-1] = '\0';
    strncpy(myclient->player_name, player_name, PLAYER_NAME_LEN);

    // Set the name of the game.
    printf("Name of your game? (max %d symbols): ", GAME_NAME_LEN-1);
    fgets(game_name, GAME_NAME_LEN, stdin);

    game_name[strlen(game_name)-1] = '\0';
    strncpy(myclient->curr_game_name, game_name, GAME_NAME_LEN);

    // Serialize the data.
    bzero(buffer, MAX_BUFFER_SIZE);
    serialize_msg_CG(buffer, msg_type, player_name, game_name, field_id);

    // Initiate sending and receiving.
    data_n = send(serv_sock, buffer, MAX_BUFFER_SIZE, 0);
    if (data_n < 0) {
        free(myclient);
        free(buffer);
        err_die("Could not send message of type CREATE GAME!");
    }

    bzero(buffer, MAX_BUFFER_SIZE);
    data_n = recv(serv_sock, buffer, MAX_BUFFER_SIZE, 0);
    if (data_n < 0) {
        free(myclient);
        free(buffer);
        err_die("Could not receive message of type CREATE GAME!");
    }

    deserialize_msg_CG_response(buffer, myclient);

    printf("Type %s\n", msg_type);
    printf("Game ID %d\n", myclient->curr_game_id);
    printf("Player ID %d\n", myclient->player_id);
    printf("Pass %s\n", myclient->player_pass);

    // sit in while loop and get notified about new players joining (max 4) 
    // until want to procees to send a START GAME request.
}

int main(int argc, char **argv) {
    struct sockaddr_in serv_addr;
    int serv_sock;
    int err;

    char *serv_ip = "127.0.0.1";

    signal(SIGINT, handle_sigint);

    // Define socket settings for connecting to the server.
    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family        = AF_INET;
    serv_addr.sin_addr.s_addr   = inet_addr(serv_ip);
    serv_addr.sin_port          = htons(PORT);

    // Establish connection to the server.
    err = connect(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (err < 0) {
        err_die("Connection to the server failed!");
    }

    // Initialize the client and the buffer.
    client_t *myclient = (client_t*)calloc(sizeof(client_t), 1);
    char *buffer = (char*)calloc(MAX_BUFFER_SIZE, 1);

    /* === The lifecycle of the race game === */

    int field_id = get_number_of_fields(buffer, myclient, serv_sock);
    create_game(buffer, myclient, serv_sock, field_id);

    // the next nodes in lifecycle...

    free(myclient);
    free(buffer);
    close(serv_sock);
    exit(EXIT_SUCCESS);
}

void handle_sigint(int sig) {
    printf("Caught signal - %d\n", sig);
    exit(EXIT_FAILURE);
}