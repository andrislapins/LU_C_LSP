#include "../include/common.h"
#include "../include/protocol.h"
#include "../include/serialization.h"
#include "../include/my_strings.h"
#include <ncurses.h>

int serv_sock;

void handle_sigint(int sig);
void send_n_recv(char* buffer);
void init_values(char *buffer, client_t *client, track_t *track);

void list_games(char *buffer) {
    char msg_type[MSG_TYPE_LEN];
    int n_games_ids, chose;

    strcpy(msg_type, "LI\0");

    bzero(buffer, MAX_BUFFER_SIZE);
    serialize_msg_LI(buffer, msg_type);

    send_n_recv(buffer);

    deserialize_msg_LI_response(buffer, &(n_games_ids));
    printf("There is/are currently %d game (ID: 1 - %d) on the server\n", n_games_ids, n_games_ids);

    /* GAME INFO */

    strncpy(msg_type, "GI\0", 3);

    bzero(buffer, MAX_BUFFER_SIZE);
    // serialize_msg_GI(buffer, msg_type);

    send_n_recv(buffer);

    // deserialize_msg_GI_response(buffer);

}

int get_number_of_fields(char* buffer, client_t *client, track_t *track) {
    char msg_type[MSG_TYPE_LEN];

    int n_field_ids, chose;
    char num_str[DIGITS_LEN];
    memset(num_str, 0, DIGITS_LEN);

    strcpy(msg_type, "NF\0");

    bzero(buffer, MAX_BUFFER_SIZE);
    serialize_msg_NF(buffer, msg_type);

    send_n_recv(buffer);

    deserialize_msg_NF_response(buffer, &(n_field_ids));

    printf("Choose a field in range from 1 to %d to get more info: ", n_field_ids);
    fgets(num_str, DIGITS_LEN, stdin);

    chose = atoi(num_str);
    if (chose <= 0 || chose > n_field_ids) {
        err_die("You provided an invalid number!");
    }
    printf("You chose: %d\n", chose);

    /* FIELD INFO */

    strncpy(msg_type, "FI\0", 3);

    bzero(buffer, MAX_BUFFER_SIZE);
    serialize_msg_FI(buffer, msg_type, chose);

    send_n_recv(buffer);

    deserialize_msg_FI_response(buffer, track);

    printf("Field ID: %d\n", track->field->ID);
    printf("Field name: %s\n", track->field->name);
    printf("Field width: %d\n", track->field->Width);
    printf("Field height: %d\n", track->field->Height);
    printf("Start line beggining x: %f, y: %f\n", track->start_line->beggining.x, track->start_line->beggining.y);
    printf("Start line end x: %f, y: %f\n", track->start_line->end.x, track->start_line->end.y);
    printf("Main line beggining x: %f, y: %f\n", track->main_line->beggining.x, track->main_line->beggining.y);
    printf("Main line end x: %f, y: %f\n", track->main_line->end.x, track->main_line->end.y);
    printf("Number of extra lines: %d\n", track->n_extra_lines);
    // NOTE: Need to figure out how to handle extra lines. 
    // (OPT: A cycle going through the number of extra lines, and in that cycle
    // create new extra line instances which to store in a global array of
    // serialization.c file. Later the data of this array pass through params
    // of deserialize_msg_FI_response();
    // for (int i = 0; i < track->n_extra_lines; i++) {
    //     printf("Extra line i beggining x: %d, y: %d\n", myfields[i](line values), myfields[i](line values));
    //     printf("Extra line i end x: %d, y: %d\n", myfields[i](line values), myfields[i](line values));
    // }
    // OPT: We migth ask for FIELD INFO for a particular field until one is chosen in a while loop.
    // or just pursue with the first asked field (almost like now);

    return track->field->ID;
}

void create_game(char *buffer, client_t *client, int field_id) {
    char msg_type[MSG_TYPE_LEN];
    char *client_name;
    char *game_name;
    int p_name_allowed = CLIENT_NAME_LEN - 1;
    int g_name_allowed = GAME_NAME_LEN - 1;

    // Initialiaze the names.
    client_name = (char*)calloc(CLIENT_NAME_LEN, 1);
    if (client_name == NULL) {
        err_die("Could not allocate memory for client name!");
    }

    game_name = (char*)calloc(GAME_NAME_LEN, 1);
    if (game_name == NULL) {
        err_die("Could not allocate memory for game name!");
    }

    strcpy(msg_type, "CG\0");

    // Set the player's name.
    printf("Your name? (max %d symbols): ", p_name_allowed);
    fgets(client_name, p_name_allowed, stdin);
    client_name[strlen(client_name)-1] = '\0';
    strncpy(client->player->name, client_name, CLIENT_NAME_LEN);

    // Set the name of the game.
    printf("Name of your game? (max %d symbols): ", g_name_allowed);
    fgets(game_name, g_name_allowed, stdin);
    game_name[strlen(game_name)-1] = '\0';
    strncpy(client->game->game->name, game_name, GAME_NAME_LEN);

    // Serialize the data.
    bzero(buffer, MAX_BUFFER_SIZE);
    serialize_msg_CG(buffer, msg_type, client_name, game_name, field_id);

    send_n_recv(buffer);

    deserialize_msg_CG_response(buffer, client);

    printf("Type %s\n", msg_type);
    printf("Game ID %d\n", client->game->ID);
    printf("Player ID %d\n", client->player->ID);
    printf("Password %s\n", client->password);

    // sit in while loop and get notified about new players joining (max 4) 
    // until want to procees to send a START GAME request.
}

int main(int argc, char **argv) {
    struct sockaddr_in serv_addr;
    int err;

    char *buffer;
    client_t *client;
    track_t *track;

    int field_id;

    signal(SIGINT, handle_sigint);

    // Define socket settings for connecting to the server.
    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family        = AF_INET;
    serv_addr.sin_addr.s_addr   = inet_addr("127.0.0.1");
    serv_addr.sin_port          = htons(PORT);

    printf("Welcome to the best race game EVER!\n");
    
    // Establish connection to the server.
    err = connect(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (err < 0) {
        err_die("Connection to the server failed!");
    }

    // Initialize the client, buffer etc.
    buffer = (char*)calloc(1, MAX_BUFFER_SIZE);
    if (buffer == NULL) {
        err_die("Could not allocate memory for buffer!");
    }

    client = (client_t*)calloc(1, sizeof(track_t));
    if (client == NULL) {
        err_die("Could not allocate memory for client!");
    }

    track = (track_t*)calloc(1, sizeof(track_t));
    if (track == NULL) {
        err_die("Could not allocate memory for field!");
    }

    init_values(buffer, client, track);

    // === The lifecycle of the race game ===

    // field_id = get_number_of_fields(buffer, client, track);
    // create_game(buffer, client, field_id);
    list_games(buffer);

    // ...

    fflush(stdout);
    fflush(stdin);
    // shutdown(serv_sock, SHUT_RDWR);
    close(serv_sock);
    printf("Goodbye!\n");
    exit(EXIT_SUCCESS);
}

void handle_sigint(int sig) {
    fflush(stdout);
    fflush(stdin);
    // shutdown(serv_sock, SHUT_RDWR);
    close(serv_sock);
    printf("Caught signal - %d\n", sig);
    exit(EXIT_FAILURE);
}

void init_values(char *buffer, client_t *client, track_t *track) {
    track->field = (struct Field*)calloc(sizeof(struct Field), 1);
    if (track->field == NULL) {
        err_die("Could not allocate memory for client's field!");
    }
    
    track->start_line = (struct Line*)calloc(sizeof(struct Line), 1);
    if (track->start_line == NULL) {
        err_die("Could not allocate memory for client's start line!");
    }
    
    track->main_line = (struct Line*)calloc(sizeof(struct Line), 1);
    if (track->main_line == NULL) {
        err_die("Could not allocate memory for client's main line!");
    }
}

void send_n_recv(char *buffer) {
    ssize_t data_n;
    data_n = send(serv_sock, buffer, MAX_BUFFER_SIZE, 0);
    if (data_n < 0) {
        err_die("Could not send message of type CREATE GAME!");
    }

    bzero(buffer, MAX_BUFFER_SIZE);
    data_n = recv(serv_sock, buffer, MAX_BUFFER_SIZE, 0);
    if (data_n < 0) {
        err_die("Could not receive message of type CREATE GAME!");
    }
}