#include "../include/common.h"
#include "../include/protocol.h"
#include "../include/serialization.h"
#include "../include/log_messages.h"
#include "../include/linked_list.h"

// NOTE: Have %s p/h to which I pass fmt([string]) func which 
// appends/prepends my format rules.
// NOTE: Handle overflows for number buffer like in create_game() func.

// LLs and variables of the main game types.
client_node_t *clients_start = NULL;
game_node_t *games_start     = NULL;
track_node_t *tracks_start   = NULL;

// The main message buffer.
char *buffer;

// Variables for global flag storing.
FILE *output;
char *ip;
int  port;

// Helper function prototypes.
void handle_sigint(int sig);
void send_n_recv(char* buffer, char *msg_type, int serv_sock);
void err_die_client(FILE *fp, char* err_msg);
void change_log_output(FILE *fp, char *path);

// Global structure initialization prototypes.
void init_client(client_t **client);
void init_game(game_t **game, int chosen_field_id);
void init_tracks();

/* The main functions for communication */

void list_games(char *buffer, client_t *client, client_t ***g_clients) {
    char msg_type[MSG_TYPE_LEN];
    char num_str[DIGITS_LEN];
    // Store array of received game IDs.
    int *gid_arr;
    int n_games, chose, g_client_count;

    /* LIST GAME */

    strcpy(msg_type, "LI\0");

    // Initialize the buffer for numbers.
    memset(num_str, '\0', DIGITS_LEN);

    memset(buffer, '\0', MAX_BUFFER_SIZE);
    serialize_msg_LI(buffer, msg_type);

    send_n_recv(buffer, msg_type, client->sock_fd);

    deserialize_msg_LI_response(buffer, msg_type, &(n_games), &gid_arr);

    // Log the response.
    log_received_LI_msg(output, msg_type, n_games, gid_arr);

    // If the are no game on the server.
    if (n_games == 0) {
        return;
    }
    
    /* GAME INFO */

    strncpy(msg_type, "GI\0", 3);

    // Reinitialize the buffer for numbers.
    memset(num_str, '\0', DIGITS_LEN);

    printf("Choose one of the game IDs to get more info: ");
    fgets(num_str, DIGITS_LEN, stdin);

    chose = atoi(num_str);
    if (chose <= 0) {
        err_die_client(output, "You provided an invalid number!");
    }

    // Reinitialize the buffer for data serialization.
    memset(buffer, '\0', MAX_BUFFER_SIZE);
    serialize_msg_GI(buffer, msg_type, chose);

    send_n_recv(buffer, msg_type, client->sock_fd);

    // Create multiple client instances on client-side.
    g_clients = malloc(MAX_CLIENTS_PER_GAME * sizeof(*g_clients));

    for(int i = 0; i < MAX_CLIENTS_PER_GAME; i++) {
        init_client(g_clients[i]);
        (*g_clients[i])->sock_fd = -1;
        (*g_clients[i])->game = client->game;
    }

    // Use client/game/track allocation and init functions from server.
    // And use push instances in the same way.
    // I guess client will have to store clients like in server.
    // But I could assign the same pointer of game/track to every client.
    // Or just have multiple clients in clients's LL.
    // Any way I will have to update just client values NOOOT client game/track
    // values specifially.
    // Thats why avoid pushing/allocating multiple games/tracks (that
    // may be the reason why client crashes).

    deserialize_msg_GI_response(buffer, msg_type, client->game, &g_client_count, *g_clients); 

    // Log the response.
    log_received_GI_msg(output, msg_type, client, g_client_count, *g_clients);

    free(gid_arr);
    // free(g_clients);
}

int get_number_of_fields(char* buffer, client_t *client) {
    char msg_type[MSG_TYPE_LEN];
    char num_str[DIGITS_LEN];
    int  n_field_ids, chose;
    track_t *track;

    /* GET_NUMBER_OF_FIELDS */

    strcpy(msg_type, "NF\0");

    memset(num_str, '\0', DIGITS_LEN);

    memset(buffer, '\0', MAX_BUFFER_SIZE);
    serialize_msg_NF(buffer, msg_type);

    send_n_recv(buffer, msg_type, client->sock_fd);

    deserialize_msg_NF_response(buffer, msg_type, &(n_field_ids));

    /* FIELD INFO */

    strncpy(msg_type, "FI\0", 3);

    // Allocate, initialize and assign the track to client.
    init_tracks(&track);
    client->game->track = track;

    // Getting the chosen field ID.
    printf("Choose a field in range from 1 to %d to get more info: ", n_field_ids);
    fgets(num_str, DIGITS_LEN, stdin);

    chose = atoi(num_str);
    if (chose <= 0 || chose > n_field_ids) {
        err_die_client(output, "You provided an invalid number!");
    }

    // Reinitialize buffer for data serialization.
    memset(buffer, '\0', MAX_BUFFER_SIZE);
    serialize_msg_FI(buffer, msg_type, chose);

    send_n_recv(buffer, msg_type, client->sock_fd);

    deserialize_msg_FI_response(buffer, msg_type, client->game->track);

    // Log the response.
    log_received_FI_msg(output, msg_type, client);

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
    char    *name_buf;
    int     p_name_allowed_size = CLIENT_NAME_LEN - 1;
    int     g_name_allowed_size = GAME_NAME_LEN - 1;
    game_t *game;

    /* CREATE GAME */

    strcpy(msg_type, "CG\0");

    // Create a new game instance on the client.
    init_game(&game, field_id);

    // Assign the game to the client.
    client->game = game;

    // Allocate and initialize the buffer where to store names.
    name_buf = malloc(MSG_BUF_LEN);
    if (name_buf == NULL) {
        err_die_client(output, "Could not allocate memory for name buffer!");
    }

    memset(name_buf, '\0', MSG_BUF_LEN);

    /* Get and set the name of the client */

    printf("Your name? (max %d symbols): ", p_name_allowed_size);
    fgets(name_buf, MSG_BUF_LEN, stdin);

    // Set the last element of name_buf to 0 for checking the length. 
    name_buf[MSG_BUF_LEN-1] = '\0';
    if (strlen(name_buf) > p_name_allowed_size) {
        err_die_client(output, "The name is too long!");
    }

    name_buf[strlen(name_buf)-1] = '\0';
    strncpy(client->player->name, name_buf, p_name_allowed_size);

    // Reinitialize the buffer for names.
    memset(name_buf, '\0', MSG_BUF_LEN);

    /* Get and set the name of the game */

    printf("Name of your game? (max %d symbols): ", g_name_allowed_size);
    fgets(name_buf, MSG_BUF_LEN, stdin);

    // Set the last element of name_buf to 0 for checking the length.
    name_buf[MSG_BUF_LEN-1] = '\0';
    if (strlen(name_buf) > p_name_allowed_size) {
        err_die_client(output, "The name is too long!");
    }

    name_buf[strlen(name_buf)-1] = '\0';
    strncpy(client->game->game_h->name, name_buf, g_name_allowed_size);

    // Serialize the data.
    memset(buffer, '\0', MAX_BUFFER_SIZE);
    serialize_msg_CG(
        buffer, msg_type, client->player->name, 
        client->game->game_h->name, field_id
    );

    send_n_recv(buffer, msg_type, client->sock_fd);

    deserialize_msg_CG_response(buffer, msg_type, client);

    // Log the response.
    log_received_CG_msg(output, msg_type, client);

    free(name_buf);

    // NOTE: Sit in while loop and get notified about new players joining (max 4) 
    // until want to procees to send a START GAME request.
}

int main(int argc, char **argv) {
    struct sockaddr_in  serv_addr;
    int                 err, flag, serv_sock;

    client_t    **g_clients;
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

    // USER-OPT-CREATE branch.
    create_game(buffer, client, field_id);
    field_id = get_number_of_fields(buffer, client);

    // USER-OPT-JOIN branch.
    list_games(buffer, client, &g_clients);

    /* Exit program gracefully */

    printf("Press ENTER to exit...");
    getchar();

    free(buffer);

    remove_all_clients(output, &clients_start);
    remove_all_tracks(output, &tracks_start);
    remove_all_games(output, &games_start);

    free(g_clients);

    printf("Goodbye!\n");
    exit(EXIT_SUCCESS);
}

/* === Helper functions === */

void handle_sigint(int sig) {
    char err_msg[ERR_MSG_LEN];

    memset(err_msg, 0, ERR_MSG_LEN);
    sprintf(err_msg, "Caught signal - %d", sig);
    
    err_die_client(output, err_msg);
}


void err_die_client(FILE *fp, char* err_msg) {
    // General message handling.
    log_time_header(fp);
    fprintf(fp, "%sERROR: %s ", ANSI_RED, err_msg);
    fprintf(fp, "(errno = %d): %s%s\n", errno, strerror(errno), ANSI_RESET_ALL);

    // Freeing memory.
    free(buffer);

    remove_all_clients(output, &clients_start);
    remove_all_tracks(output, &tracks_start);
    remove_all_games(output, &games_start);

    exit(EXIT_FAILURE);
}

void change_log_output(FILE *output, char *path) {
    output = fopen(path, "w");
    if (output == NULL) {
        err_die_client(stdout, "Error opening the given log file!");
    }
}

void send_n_recv(char *buffer, char *msg_type, int serv_sock) {
    char    send_err_msg[ERR_MSG_LEN];
    char    recv_err_msg[ERR_MSG_LEN];
    ssize_t data_n;

    data_n = send(serv_sock, buffer, MAX_BUFFER_SIZE, 0);
    if (data_n < 0) {
        sprintf(send_err_msg, "Could not send message of type %s!", msg_type);
        err_die_client(output, send_err_msg);
    }

    memset(buffer, 0, MAX_BUFFER_SIZE);
    data_n = recv(serv_sock, buffer, MAX_BUFFER_SIZE, 0);
    if (data_n < 0) {
        sprintf(recv_err_msg, "Could not receive message of type %s!", msg_type);
        err_die_client(output, recv_err_msg);
    }
}

void init_client(client_t **client) {
    // Client struct allocation.
    *client = malloc(sizeof(client_t));
    if (*client == NULL) {
        err_die_client(output, "Could not allocate memory for a client!");
    }

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

    // Initialising the char arrays.
    memset((*client)->ip, '\0', IP_LEN);
    memset((*client)->player->name, '\0', CLIENT_NAME_LEN);

    // Allocated field definitions.
    (*client)->player->ID           = 0;
    (*client)->player->angle        = 0.0f;
    (*client)->player->speed        = 0.0f;
    (*client)->player->acceleration = 0.0f;
    (*client)->player->laps         = 0;
    (*client)->player->position.x   = 0;
    (*client)->player->position.y   = 0;

    // Save this client in the global clients list.
    push_client(&clients_start, client);
}

void init_game(game_t **game, int chosen_field_id) {
    track_t *chosen_track;

    // Game struct allocation.
    *game = malloc(sizeof(game_t));
    if (*game == NULL) {
        err_die_client(output, "Could not allocate memory for game!");
    }

    // Game header allocation.
    (*game)->game_h = malloc(sizeof(struct Game));
    if ((*game)->game_h == NULL) {
        err_die_client(output, "Could not allocate memory for game!");
    }

    // Initialising the char arrays.
    memset((*game)->game_h->name, '\0', GAME_NAME_LEN);

     // Allocated field definitions.
    (*game)->ID                        = 0;
    (*game)->game_h->status            = 0;
    (*game)->game_h->WinnerPlayerID    = -1;

    // Assigning a field to the game.
    chosen_track = get_track_by_id(&tracks_start, chosen_field_id);
    if (chosen_track == NULL) {
        err_die_client(output, "Could not find the requested field!");
    }

    (*game)->track = chosen_track;

    // Save this game in the global games list.
    push_game(&games_start, game);
}

void init_tracks(track_t **track) {
    // Track struct allocation.
    *track = malloc(sizeof(track_t));
    if (track == NULL) {
        err_die_client(output, "Could not allocate memory for a track!");
    }

    // Track field allocation.
    (*track)->field = malloc(sizeof(struct Field));
    if (track == NULL) {
        err_die_client(output, "Could not allocate memory for a field!");
    }

    // Track start line allocation.
    (*track)->start_line = malloc(sizeof(struct Line));
    if ((*track)->start_line == NULL) {
        err_die_client(output, "Could not allocate memory for a field's start line!");
    }

    // Tracks main line allocation.
    (*track)->main_line = malloc(sizeof(struct Line));
    if ((*track)->main_line == NULL) {
        err_die_client(output, "Could not allocate memory for a field's main line!");
    }

    // Initialising the char arrays.
    memset((*track)->field->name, '\0', FIELD_NAME_LEN);

    // Allocated field definitions.
    (*track)->n_extra_lines            = 0;

    (*track)->field->ID                = 0;
    (*track)->field->Width             = 0;
    (*track)->field->Height            = 0;

    (*track)->start_line->beggining.x  = 0.0f;
    (*track)->start_line->beggining.y  = 0.0f;
    (*track)->start_line->end.x        = 0.0f;
    (*track)->start_line->end.y        = 0.0f;

    (*track)->main_line->beggining.x   = 0.0f;
    (*track)->main_line->beggining.y   = 0.0f;
    (*track)->main_line->end.x         = 0.0f;
    (*track)->main_line->end.y         = 0.0f;

    // Save this track in the global tracks list.
    push_track(&tracks_start, track);
}

// void init_client(client_t **client) { // Initial client initialization.
//     // Client struct allocation.
//     *client = malloc(sizeof(client_t));
//     if (*client == NULL) {
//         err_die_client(output, "Could not allocate memory for client!");
//     }

//     // Client IP address allocation.
//     (*client)->ip = malloc(IP_LEN);
//     if ((*client)->ip == NULL) {
//         err_die_client(output, "Could not allocate memory for client's IP!");
//     }

//     // Client password allocation.
//     (*client)->password = malloc(CLIENT_PASS_LEN);
//     if ((*client)->password == NULL) {
//         err_die_client(output, "Could not allocate memory for client's password!");
//     }

//     // Client player info struct allocation.
//     (*client)->player = malloc(sizeof(struct Player_info));
//     if ((*client)->player == NULL) {
//         err_die_client(output, "Could not allocate memory for client's player!");
//     }

//     // Define client's game struct.
//     (*client)->game = malloc(sizeof(game_t));
//     if ((*client)->game == NULL) {
//         err_die_client(output, "Could not allocate memory for client's game!");
//     }

//     // Define client's game header struct.
//     (*client)->game->game_h = malloc(sizeof(struct Game));
//     if ((*client)->game->game_h == NULL) {
//         err_die_client(output, "Could not allocate memory for client's game header!");
//     }

//     // Define client's game track.
//     (*client)->game->track = malloc(sizeof(track_t));
//     if ((*client)->game->track == NULL) {
//         err_die_client(output, "Could not allocate memory for client's game track!");
//     }

//     (*client)->game->track->field = malloc(sizeof(struct Field));
//     if ((*client)->game->track->field == NULL) {
//         err_die_client(output, "Could not allocate memory for client's game track field!");
//     }

//     // Define client's game start line.
//     (*client)->game->track->start_line = malloc(sizeof(struct Line));
//     if ((*client)->game->track->start_line == NULL) {
//         err_die_client(output, "Could not allocate memory for client's game track start line!");
//     }

//     // Define client's game main line.
//     (*client)->game->track->main_line = malloc(sizeof(struct Line));
//     if ((*client)->game->track->main_line == NULL) {
//         err_die_client(output, "Could not allocate memory for client's game track main line!");
//     }

//     // Initialising the char arrays.
//     memset((*client)->ip, '\0', IP_LEN);
//     memset((*client)->player->name, '\0', CLIENT_NAME_LEN);
//     memset((*client)->game->game_h->name, '\0', GAME_NAME_LEN);

//     // Save client, game, track to the globally accesable lists.
//     push_client(&clients_start, client);
//     push_game(&games_start, &((*client)->game));
//     push_track(&tracks_start, &((*client)->game->track));
// }