#include "../include/common.h"
#include "../include/protocol.h"
#include "../include/serialization.h"
#include "../include/log_messages.h"
#include "../include/linked_list.h"

// NOTE: Have %s p/h to which I pass fmt([string]) func which 
// appends/prepends my format rules.

// LLs and variables of the main game types.
client_node_t *clients_start = NULL;
unsigned int client_count    = 0; // NOTE:? Is this count needed for client.

game_node_t *games_start     = NULL;
unsigned int game_count      = 0; // NOTE:? Is this count needed for client.

track_node_t *tracks_start   = NULL;
unsigned int track_count     = 0; // NOTE:? Is this count needed for client.

// The main message buffer.
char *buffer;

// Variables for global flag storing.
static FILE *output;
static char *ip;
static int  port;

// Helper function prototypes.
static void handle_sigint(int sig);
static void send_n_recv(char* buffer, char *msg_type, int serv_sock);
static void err_die_client(FILE *fp, char* err_msg);
static void change_log_output(FILE *fp, char *path);
static void help();

// Global structure initialization prototypes.
void init_client(client_t **client);

void list_games(char *buffer) {
    // char msg_type[MSG_TYPE_LEN];
    // char num_str[DIGITS_LEN];
    // int *game_ids; // store array of received ids.
    // int n_games, chose;

    // memset(num_str, 0, DIGITS_LEN);
    // strcpy(msg_type, "LI\0");

    // memset(buffer, 0, MAX_BUFFER_SIZE);
    // serialize_msg_LI(buffer, msg_type);

    // send_n_recv(buffer, msg_type, client->sock_fd);

    // deserialize_msg_LI_response(buffer, &(n_games));
    // printf("There is/are currently %d game on the server\n", n_games);
    // printf("Choose a ... to get more info: ");
    // fgets(num_str, DIGITS_LEN, stdin);

    // chose = atoi(num_str);
    // if (chose <= 0) {
    //     err_die_client(output, "You provided an invalid number!");
    // }
    // printf("You chose game w/ ID: %d\n", chose);

    /* GAME INFO */

    // strncpy(msg_type, "GI\0", 3);

    // memset(buffer, 0, MAX_BUFFER_SIZE);
    // serialize_msg_GI(buffer, msg_type);

    // send_n_recv(buffer, msg_type, client->sock_fd);

    // deserialize_msg_GI_response(buffer);
}

int get_number_of_fields(char* buffer, client_t *client) {
    char msg_type[MSG_TYPE_LEN];
    char num_str[DIGITS_LEN];
    int  n_field_ids, chose;

    memset(num_str, '\0', DIGITS_LEN);
    strcpy(msg_type, "NF\0");

    memset(buffer, '\0', MAX_BUFFER_SIZE);
    serialize_msg_NF(buffer, msg_type);

    send_n_recv(buffer, msg_type, client->sock_fd);

    deserialize_msg_NF_response(buffer, &(n_field_ids));

    printf("Choose a field in range from 1 to %d to get more info: ", n_field_ids);
    fgets(num_str, DIGITS_LEN, stdin);

    chose = atoi(num_str);
    if (chose <= 0 || chose > n_field_ids) {
        err_die_client(output, "You provided an invalid number!");
    }

    printf("You chose field w/ ID: %d\n", chose);

    /* FIELD INFO */

    strncpy(msg_type, "FI\0", 3);

    memset(buffer, '\0', MAX_BUFFER_SIZE);
    serialize_msg_FI(buffer, msg_type, chose);

    send_n_recv(buffer, msg_type, client->sock_fd);

    deserialize_msg_FI_response(buffer, client->game->track);

    printf("%sField ID%s: %d\n", ANSI_GREEN, ANSI_RESET_ALL, client->game->track->field->ID);
    printf("%sField name%s: %s\n", ANSI_GREEN, ANSI_RESET_ALL, client->game->track->field->name);
    printf("%sField width%s: %d\n", ANSI_GREEN, ANSI_RESET_ALL, client->game->track->field->Width);
    printf("%sField height%s: %d\n", ANSI_GREEN, ANSI_RESET_ALL, client->game->track->field->Height);
    printf("%sStart line beggining%s x: %f, y: %f\n",
            ANSI_GREEN, ANSI_RESET_ALL, 
            client->game->track->start_line->beggining.x, 
            client->game->track->start_line->beggining.y
    );
    printf("%sStart line end%s x: %f, y: %f\n",
            ANSI_GREEN, ANSI_RESET_ALL,
            client->game->track->start_line->end.x, 
            client->game->track->start_line->end.y
    );
    printf("%sMain line beggining%s x: %f, y: %f\n", 
            ANSI_GREEN, ANSI_RESET_ALL,
            client->game->track->main_line->beggining.x, 
            client->game->track->main_line->beggining.y
    );
    printf("%sMain line end%s x: %f, y: %f\n",
            ANSI_GREEN, ANSI_RESET_ALL,
            client->game->track->main_line->end.x, 
            client->game->track->main_line->end.y
    );
    printf("%sNumber of extra lines%s: %d\n", 
            ANSI_GREEN, ANSI_RESET_ALL,
            client->game->track->n_extra_lines
    );
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

    return client->game->track->field->ID;
}

void create_game(char *buffer, client_t *client, int field_id) {
    char    msg_type[MSG_TYPE_LEN];
    char    *client_name;
    char    *game_name;
    int     p_name_allowed_size = CLIENT_NAME_LEN - 1;
    int     g_name_allowed_size = GAME_NAME_LEN - 1;

    // Initialiaze the names.
    client_name = malloc(CLIENT_NAME_LEN);
    if (client_name == NULL) {
        err_die_client(output, "Could not allocate memory for client name!");
    }

    game_name = malloc(GAME_NAME_LEN);
    if (game_name == NULL) {
        err_die_client(output, "Could not allocate memory for game name!");
    }

    memset(client_name, '\0', CLIENT_NAME_LEN);
    memset(game_name,   '\0', GAME_NAME_LEN);

    strcpy(msg_type, "CG\0");

    // Get and set the player's name.
    printf("Your name? (max %d symbols): ", p_name_allowed_size);
    fgets(client_name, p_name_allowed_size, stdin);
    client_name[strlen(client_name)-1] = '\0';
    strncpy(client->player->name, client_name, CLIENT_NAME_LEN);

    // Get and set the name of the game.
    printf("Name of your game? (max %d symbols): ", g_name_allowed_size);
    fgets(game_name, g_name_allowed_size, stdin);
    game_name[strlen(game_name)-1] = '\0';
    strncpy(client->game->game_h->name, game_name, GAME_NAME_LEN);

    // Serialize the data.
    memset(buffer, '\0', MAX_BUFFER_SIZE);
    serialize_msg_CG(buffer, msg_type, client_name, game_name, field_id);

    send_n_recv(buffer, msg_type, client->sock_fd);

    deserialize_msg_CG_response(buffer, client);

    printf("Type %s\n",         msg_type);
    printf("Game ID %d\n",      client->game->ID);
    printf("Player ID %d\n",    client->player->ID);
    printf("Password %s\n",     client->password);

    free(client_name);
    free(game_name);

    // NOTE: Sit in while loop and get notified about new players joining (max 4) 
    // until want to procees to send a START GAME request.
}

int main(int argc, char **argv) {
    struct sockaddr_in  serv_addr;
    int                 err, flag, serv_sock;

    client_t    *client;
    int         field_id;

    signal(SIGINT, handle_sigint);

    // Default flag values.
    output = stdout;
    ip     = "127.0.0.1";
    port   = PORT;

    while ((flag = getopt(argc, argv, "a:p:l:h")) != -1) {
        switch (flag) {
            case 'a':
                strncpy(ip, optarg, IP_LEN);
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'l':
                change_log_output(output, optarg);
                break;
            case 'h':
                help();
                exit(EXIT_SUCCESS);
            default:
                help();
                exit(EXIT_FAILURE);
        }
    }

    // Define socket settings for connecting to the server.
    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family        = AF_INET;
    serv_addr.sin_addr.s_addr   = inet_addr(ip);
    serv_addr.sin_port          = htons(port);

    // Establish connection to the server.
    err = connect(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (err < 0) {
        err_die_client(output, "Connection to the server failed!");
    }

    // === Client/Game has started ===

    printf("Welcome to the best race game EVER!\n");

    // Allocate and initialize the buffer.
    buffer = malloc(MAX_BUFFER_SIZE);
    if (buffer == NULL) {
        err_die_client(output, "Could not allocate memory for buffer!");
    }

    memset(buffer, '\0', MAX_BUFFER_SIZE);
    
    // Define client and all its inner structures.
    init_client(&client);
    client->sock_fd = serv_sock;

    /* The lifecycle of the race game */

    field_id = get_number_of_fields(buffer, client);
    create_game(buffer, client, field_id);
    list_games(buffer);

    // ...

    printf("Press ENTER to exit...\n");
    getchar();

    /* Exit program gracefully */

    free(buffer);

    remove_all_clients(output, &clients_start);
    remove_all_tracks(output, &tracks_start);
    remove_all_games(output, &games_start);

    printf("Goodbye!\n");
    exit(EXIT_SUCCESS);
}

/* === Helper functions === */

void handle_sigint(int sig) {
    char err_msg[ERR_MSG_LEN];

    memset(err_msg, 0, ERR_MSG_LEN);
    sprintf(err_msg, "\nCaught signal - %d\n", sig);
    
    err_die_client(output, err_msg);
}


void err_die_client(FILE *fp, char* err_msg) {
    free(buffer);

    remove_all_clients(output, &clients_start);
    remove_all_tracks(output, &tracks_start);
    remove_all_games(output, &games_start);

    // General message handling.
    log_time_header(fp);
    fprintf(fp, "%sERROR: %s ", ANSI_RED, err_msg);
    fprintf(fp, "(errno = %d): %s%s\n", errno, strerror(errno), ANSI_RESET_ALL);

    exit(EXIT_FAILURE);
}

void change_log_output(FILE *output, char *path) {
    output = fopen(path, "w");
    if (output == NULL) {
        err_die_client(stdout, "Error opening the given log file!");
    }
}

void help() {
    printf("Usage format: server.exe");
    printf("[-a ip-address] [-p port-number] [-l output-file] [-h]\n");
    printf("Default values: -a=127.0.0.1, -p=%d, -l=output\n", PORT);
}

void send_n_recv(char *buffer, char *msg_type, int serv_sock) {
    ssize_t data_n;
    char    send_err_msg[ERR_MSG_LEN];
    char    recv_err_msg[ERR_MSG_LEN];

    sprintf(send_err_msg, "Could not send message of type %s!", msg_type);
    sprintf(recv_err_msg, "Could not receive message of type %s!", msg_type);

    data_n = send(serv_sock, buffer, MAX_BUFFER_SIZE, 0);
    if (data_n < 0) {
        err_die_client(output, send_err_msg);
    }

    memset(buffer, 0, MAX_BUFFER_SIZE);
    data_n = recv(serv_sock, buffer, MAX_BUFFER_SIZE, 0);
    if (data_n < 0) {
        err_die_client(output, recv_err_msg);
    }
}

void init_client(client_t **client) {
    // Client struct allocation.
    *client = malloc(sizeof(client_t));
    if (*client == NULL) {
        err_die_client(output, "Could not allocate memory for client!");
    }
    // memset(*client, 0, sizeof(client_t));

    // Client IP address allocation.
    (*client)->ip = malloc(IP_LEN);
    if ((*client)->ip == NULL) {
        err_die_client(output, "Could not allocate memory for client's IP!");
    }

    // Client password allocation.
    (*client)->password = malloc(CLIENT_PASS_LEN);
    if ((*client)->password == NULL) {
        err_die_client(output, "Could not allocate memory for client's password!");
    }


    // Client player info struct allocation.
    (*client)->player = malloc(sizeof(struct Player_info));
    if ((*client)->player == NULL) {
        err_die_client(output, "Could not allocate memory for client's player!");
    }

    // memset((*client)->player, 0, sizeof(struct Player_info));

    // Define client's game struct.
    (*client)->game = malloc(sizeof(game_t));
    if ((*client)->game == NULL) {
        err_die_client(output, "Could not allocate memory for client's game!");
    }
    // memset((*client)->game, 0, sizeof(game_t));

    // Define client's game header struct.
    (*client)->game->game_h = malloc(sizeof(struct Game));
    if ((*client)->game->game_h == NULL) {
        err_die_client(output, "Could not allocate memory for client's game header!");
    }
    // memset((*client)->game->game_h, 0, sizeof(struct Game));

    // Define client's game track.
    (*client)->game->track = malloc(sizeof(track_t));
    if ((*client)->game->track == NULL) {
        err_die_client(output, "Could not allocate memory for client's game track!");
    }
    // memset((*client)->game->track, 0, sizeof(track_t));

    (*client)->game->track->field = malloc(sizeof(struct Field));
    if ((*client)->game->track->field == NULL) {
        err_die_client(output, "Could not allocate memory for client's game track field!");
    }
    // memset((*client)->game->track->field, 0, sizeof(struct Field));

    // Define client's game start line.
    (*client)->game->track->start_line = malloc(sizeof(struct Line));
    if ((*client)->game->track->start_line == NULL) {
        err_die_client(output, "Could not allocate memory for client's game track start line!");
    }
    // memset((*client)->game->track->start_line, 0, sizeof(struct Line));

    // Define client's game main line.
    (*client)->game->track->main_line = malloc(sizeof(struct Line));
    if ((*client)->game->track->main_line == NULL) {
        err_die_client(output, "Could not allocate memory for client's game track main line!");
    }
    // memset((*client)->game->track->main_line, 0, sizeof(struct Line));

    memset((*client)->ip, '\0', IP_LEN);
    memset((*client)->player->name, '\0', CLIENT_NAME_LEN);

    // Increase the count of players on the server.
    client_count++;

    // Save client, game, track to the globally accesable lists.
    push_client(&clients_start, client);
    push_game(&games_start, &((*client)->game));
    push_track(&tracks_start, &((*client)->game->track));
}