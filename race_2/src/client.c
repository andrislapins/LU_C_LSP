#include "../include/common.h"
#include "../include/protocol.h"
#include "../include/serialization.h"
#include "../include/my_strings.h"
#include <ncurses.h>

void handle_sigint(int sig);

int get_number_of_fields(char* buffer, client_t *myclient, field_t *myfield, int serv_sock) {
    int n_field_ids, chose;
    ssize_t data_n;
    char msg_type[] = "NF";
    char *num_str = (char*)calloc(DIGITS_LEN, 1);

    // Serialize the data.
    bzero(buffer, MAX_BUFFER_SIZE);
    serialize_msg_NF(buffer, msg_type);

    // Initiate sending and receiving.
    data_n = send(serv_sock, buffer, MAX_BUFFER_SIZE, 0);
    if (data_n < 0) {
        err_die("Could not send message of type NUMBER OF FIELDS!");
    }

    bzero(buffer, MAX_BUFFER_SIZE);
    data_n = recv(serv_sock, buffer, MAX_BUFFER_SIZE, 0);
    if (data_n < 0) {
        err_die("Could not receive message of type NUMBER OF FIELDS!");
    }

    deserialize_msg_NF_response(buffer, &(n_field_ids));

    printf("Choose a field in range from 1 to %d to get more info: ", n_field_ids);
    fgets(num_str, DIGITS_LEN, stdin);

    chose = atoi(num_str);
    if (chose <= 0 || chose > n_field_ids) {
        err_die("You provided an invalid number!");
    }
    printf("You chose: %d\n", chose);

    /* FIELD INFO */

    strncpy(msg_type, "FI", 2);
    bzero(buffer, MAX_BUFFER_SIZE);
    serialize_msg_FI(buffer, msg_type, chose);

    data_n = send(serv_sock, buffer, MAX_BUFFER_SIZE, 0);
    if (data_n < 0) {
        err_die("Could not send message of type FIELD INFO!");
    }

    bzero(buffer, MAX_BUFFER_SIZE);
    data_n = recv(serv_sock, buffer, MAX_BUFFER_SIZE, 0);
    if (data_n < 0) {
        err_die("Could not receive message of type FIELD INFO!");
    }

    print_array_in_hex("buffer before", buffer, MAX_BUFFER_SIZE);

    deserialize_msg_FI_response(buffer, myfield);

    print_array_in_hex("buffer after", buffer, MAX_BUFFER_SIZE);

    printf("Field ID: %d\n", myfield->field->ID);
    printf("Field name: %s\n", myfield->field->name);
    printf("Field width: %d\n", myfield->field->Width);
    printf("Field height: %d\n", myfield->field->Height);
    printf("Start line beggining x: %f, y: %f\n", myfield->start_line->beggining.x, myfield->start_line->beggining.y);
    printf("Start line end x: %f, y: %f\n", myfield->start_line->end.x, myfield->start_line->end.y);
    printf("Main line beggining x: %f, y: %f\n", myfield->main_line->beggining.x, myfield->main_line->beggining.y);
    printf("Main line end x: %f, y: %f\n", myfield->main_line->end.x, myfield->main_line->end.y);
    printf("Number of extra lines: %d\n", myfield->n_extra_lines);
    // NOTE: Need to figure out how to handle extra lines. 
    // (OPT: A cycle going through the number of extra lines, and in that cycle
    // create new extra line instances which to store in a global array of
    // serialization.c file. Later the data of this array pass through params
    // of deserialize_msg_FI_response();
    // for (int i = 0; i < myfield->n_extra_lines; i++) {
    //     printf("Extra line i beggining x: %d, y: %d\n", myfields[i](line values), myfields[i](line values));
    //     printf("Extra line i end x: %d, y: %d\n", myfields[i](line values), myfields[i](line values));
    // }
    // OPT: We migth ask for FIELD INFO for a particular field until one is chosen in a while loop.
    // or just pursue with the first asked field (almost like now);

    return myfield->field->ID;
}

void create_game(char *buffer, client_t *myclient, int serv_sock, int field_id) {
    ssize_t data_n;

    char msg_type[] = "CG";
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
        err_die("Could not send message of type CREATE GAME!");
    }

    bzero(buffer, MAX_BUFFER_SIZE);
    data_n = recv(serv_sock, buffer, MAX_BUFFER_SIZE, 0);
    if (data_n < 0) {
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
    // Variables typically for socket programmin.
    struct sockaddr_in serv_addr;
    int serv_sock;
    int err;
    char *serv_ip = "127.0.0.1";
    
    // Variables typically for the race game.
    int field_id;
    char *buffer;
    client_t *myclient;
    field_t *myfield;

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
    buffer = (char*)calloc(MAX_BUFFER_SIZE, 1);
    myclient = (client_t*)calloc(sizeof(client_t), 1);
    myfield = (field_t*)calloc(sizeof(field_t), 1);

    //The lifecycle of the race game.
    field_id = get_number_of_fields(buffer, myclient, myfield, serv_sock);
    create_game(buffer, myclient, serv_sock, field_id);

    // ...

    exit(EXIT_SUCCESS);
}

void handle_sigint(int sig) {
    printf("Caught signal - %d\n", sig);
    exit(EXIT_FAILURE);
}