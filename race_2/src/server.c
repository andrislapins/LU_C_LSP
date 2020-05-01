#include "../include/common.h"
#include "../include/protocol.h"
#include "../include/serialization.h"
#include "../include/log_messages.h"
#include "../include/linked_list.h"

// NOTE: have default values like IP, port, log_output but add
// store them here globally + help flags/options.
// For log_ouput I should have a seperate func to create/overwrite log file.
// Use the special flag handling library!

// Lists of the main types for manipulating w/ game data.
client_node_t *clients_start = NULL;
static unsigned int client_count = 0;
static int client_id = 0;

track_node_t *tracks_start = NULL;
static unsigned int track_count = 0;
static int track_id = 0;

game_node_t *games_start = NULL;
static unsigned int game_count = 0;
static int game_id = 0;

// Sockets. // NOTE: (do smth according to thread practices)
int listen_socket, client_socket;

// Variables for global flag storing.
FILE *output;
char *ip;
int port;

// Helper function prototypes.
void handle_sigint(int sig);
void change_log_output(FILE *fp, char *path);
void help();

// Global structure initialization prototypes.
void init_tracks_once();
void init_game(game_t **game, client_t *client);
void init_client(client_t **client, int client_socket, struct sockaddr_in client_addr);

void create_game_response(char *buffer, client_t *client) {
    game_t *game;

    deserialize_msg_CG(buffer, client);

    // Create a new game.
    init_game(&game, client);

    // Assign the new game to the client, and create a password for the client.
    client->game = game;
    generate_password(client);

    // Set a response back to the client.
    bzero(buffer, MAX_BUFFER_SIZE);
    serialize_msg_CG_response(buffer, client);

    log_create_game_response(output, client);
}

void get_number_of_fields_response(char *buffer, client_t *client) {
    // Set a response back to the client.
    bzero(buffer, MAX_BUFFER_SIZE);
    serialize_msg_NF_response(buffer, track_count);

    log_get_number_of_fields_response(output, client);
}

void field_info_response(char *buffer, client_t *client) {
    int chose;

    // Handling the message from a client.
    deserialize_msg_FI(buffer, &chose);
    
    // Set a response back to the client.
    bzero(buffer, MAX_BUFFER_SIZE);
    serialize_msg_FI_response(buffer, get_track_by_id(tracks_start, chose));

    log_field_info_response(output, client, &chose);
}

void list_games_response(char *buffer, client_t *client) {
    // Set a response back to the client.
    bzero(buffer, MAX_BUFFER_SIZE);
    serialize_msg_LI_response(buffer, game_count);

    log_list_games_response(output, client);
}

void game_info_response(char *buffer, client_t *client) {
    //
}

void handle_message(char *buffer, client_t *client) {
    char msg_type[MSG_TYPE_LEN] = { buffer[0], buffer[1], '\0' };
    buffer = buffer + 3; // Pass the point of buffer after msg_type.
    log_recvd_msg_type(output, client, msg_type);

    if (strcmp(msg_type, "CG") == 0) {
        create_game_response(buffer, client);
    } else if (strcmp(msg_type, "NF") == 0) {
        get_number_of_fields_response(buffer, client);
    } else if (strcmp(msg_type, "FI") == 0) {
        field_info_response(buffer, client);
    } else if (strcmp(msg_type, "LI") == 0) {
        list_games_response(buffer, client);
    } else if (strcmp(msg_type, "GI") == 0) {
        game_info_response(buffer, client);
    } else {
        printf("This message type is unknown!\n");
        // NOTE: Send an "error type" to kill the client?
        // NOTE: Handle this here and on client side (ex. send this error message to the client).
    }
}

void handle_client(client_t *client) {
    char *buffer;
    int leave_flag = 0;
    int data_n;
    client_count++;

    buffer = (char*)malloc(MAX_BUFFER_SIZE);
    if (buffer == NULL) {
        err_die(output, "Could not allocate memory for a buffer!");
    }
    memset(buffer, 0, MAX_BUFFER_SIZE);

    while(1) {
        if (leave_flag) {
            break;
        }

        data_n = recv(client->sock_fd, buffer, MAX_BUFFER_SIZE, 0);
        if (data_n > 0) {
            if (strlen(buffer) > 0) {
                handle_message(buffer, client);
                // Passing buffer + 3 in order to NOT send the msg_type.
                data_n = send(client->sock_fd, buffer+3, MAX_BUFFER_SIZE, 0);
                if (data_n < 0) {
                    printf("An error occurred sending a message!\n");
                    leave_flag = 1;
                }
            }
        } else if (data_n == 0) {
            printf("Lost connection with %s\n", from_who(client));
            leave_flag = 1;
        } else {
            printf("An error occurred receiving a message!\n");
            leave_flag = 1;
        }

        memset(buffer, 0, MAX_BUFFER_SIZE);
    }

    close(client->sock_fd);
    free(buffer);
    free(client);
    client_count--;
}

int main(int argc, char **argv) {
    struct sockaddr_in  serv_addr;
    struct sockaddr_in  client_addr;
    socklen_t           client_addr_len;
    int err, option, flag;

    client_t *client;

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
            case 'l':
                change_log_output(output, optarg);
            case 'h':
            default:
                help();
                exit(EXIT_FAILURE);
        }
    }

    // Define the socket settings for listening.
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family        = AF_INET;
    serv_addr.sin_addr.s_addr   = inet_addr(ip); // NOTE:? Should it be INADDR_ANY
    serv_addr.sin_port          = htons(port);

    option = 1;
    err = setsockopt(listen_socket, SOL_SOCKET, SO_REUSEPORT | SO_REUSEADDR, (char*)&option, sizeof(option));
    if (err < 0) {
        err_die(output, "Setting options on socket failed!");
    }

    err = bind(listen_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (err < 0) {
        err_die(output, "Binding failed!");
    }

    err = listen(listen_socket, MAX_CLIENTS);
    if (err < 0) {
        err_die(output, "Listening failed!");
    }

    // === Server/Game has started === //

    fprintf(output, "%sServer is on...%s\n", ANSI_BLINK, ANSI_RESET_ALL);

    init_tracks_once();

    // Initiliaze various client values.
    client_addr_len = sizeof(client_addr);
    client_socket   = accept(listen_socket, (struct sockaddr*)&client_addr, &client_addr_len);
    init_client(&client, client_socket, client_addr);

    // NOTE: Manage in a thread.
    handle_client(client);

    shutdown(listen_socket, SHUT_RDWR);
    shutdown(client_socket, SHUT_RDWR);
    printf("Goodbye!\n");
    exit(EXIT_SUCCESS);
}

/* === Non-communication specific/ Helper functions === */

void handle_sigint(int sig) {
    fprintf(output, "%s", ANSI_RESET_ALL);
    shutdown(listen_socket, SHUT_RDWR);
    shutdown(client_socket, SHUT_RDWR);
    printf("-Caught signal - %d\n", sig);
    exit(EXIT_FAILURE);
}

void change_log_output(FILE *fp, char *path) {
    fp = fopen(path, "w");
    if (fp == NULL) {
        err_die(stdout, "Error opening the given log file!");
    }
}

void help() {
    printf("Usage format: server.exe");
    printf("[-a ip-address] [-p port-number] [-l output-file] [-h]\n");
    printf("Default values: -a=127.0.0.1, -p=%d, -l=stdout\n", PORT);
}
// NOTE:? Make somehow these fields read-only (can write to their mem pool, 
// but can not change the pointers).
void init_tracks_once() {
    track_t *track = (track_t*)malloc(sizeof(track_t));
    if (track == NULL) {
        err_die(output, "Could not allocate memory for a track!");
    }
    memset(track, 0, sizeof(track_t));

    // Initialize the track's field.
    track->field = (struct Field*)malloc(sizeof(struct Field));
    if (track == NULL) {
        err_die(output, "Could not allocate memory for a field!");
    }
    memset(track->field, 0, sizeof(struct Field));

    track->field->ID        = ++track_id;
    track->field->Width     = 46;
    track->field->Height    = 12;
    
    memset(track->field->name, 0, FIELD_NAME_LEN);
    strcpy(track->field->name, "Grand Tour");

    // Initialize the track's start line.
    track->start_line = (struct Line*)malloc(sizeof(struct Line));
    if (track->start_line == NULL) {
        err_die(output, "Could not allocate memory for a field's start line!");
    }
    memset(track->start_line, 0, sizeof(struct Line));

    track->start_line->beggining.x  = 15.1;
    track->start_line->beggining.y  = 2.1;
    track->start_line->end.x        = 15.1;
    track->start_line->end.y        = 5.1;

    // Initialize the track's main line.
    track->main_line = (struct Line*)malloc(sizeof(struct Line));
    if (track->main_line == NULL) {
        err_die(output, "Could not allocate memory for a field's main line!");
    }
    memset(track->main_line, 0, sizeof(struct Line));

    track->main_line->beggining.x   = 10.1;
    track->main_line->beggining.y   = 5.1;
    track->main_line->end.x         = 25.1;
    track->main_line->end.y         = 6.1;

    // Initialize the track's extra lines.
    track->n_extra_lines            = 0;

    // Save this track in the global tracks list.
    track_count++;
    push_track(&tracks_start, &track);
}

void init_game(game_t **game, client_t *client) {
    *game = (game_t*)malloc(sizeof(game_t));
    if (*game == NULL) {
        err_die(output, "Could not allocate memory for game!");
    }
    memset(*game, 0, sizeof(game_t));

    (*game)->ID                        = ++game_id;

    (*game)->game_h = (struct Game*)malloc(sizeof(struct Game));
    memset((*game)->game_h, 0, sizeof(struct Game));

    (*game)->game_h->status            = 0;
    (*game)->game_h->WinnerPlayerID    = 0;
    memset((*game)->game_h->name, 0, GAME_NAME_LEN);
    strcpy((*game)->game_h->name, client->game->game_h->name);

    // Save the chosen track for the new game.
    (*game)->track = (track_t*)malloc(sizeof(track_t));
    memset((*game)->track, 0, sizeof(track_t));

    (*game)->track = get_track_by_id(tracks_start, client->game->track->field->ID);

    // Save this game in the global games list.
    game_count++;
    push_game(&games_start, game);
}

void init_client(client_t **client, int client_socket, struct sockaddr_in client_addr) {
    *client = (client_t*)malloc(sizeof(client_t));
    if (*client == NULL) {
        err_die(output, "Could not allocate memory for a client!");
    }
    memset(*client, 0, sizeof(client_t));

    // Initialize the client's socket values.
    (*client)->address      = client_addr;
    (*client)->sock_fd      = client_socket;
    memset((*client)->ip, 0, sizeof(IP_LEN));
    strcpy((*client)->ip, ip_addr(client_addr));

    // Initialize the client's player struct.
    (*client)->player = (struct Player_info*)malloc(sizeof(struct Player_info));
    if ((*client)->player == NULL) {
        err_die(output, "Could not allocate memory for client's player!");
    }
    memset((*client)->player, 0, sizeof(struct Player_info));

    (*client)->player->ID = ++client_id;
    memset((*client)->player->name, 0, GAME_NAME_LEN);

    // Initialize the client's game struct.
    (*client)->game = (game_t*)malloc(sizeof(game_t));
    if ((*client)->game == NULL) {
        err_die(output, "Could not allocate memory for client's game!");
    }
    memset((*client)->game, 0, sizeof(game_t));

    (*client)->game->game_h = (struct Game*)malloc(sizeof(struct Game));
    if ((*client)->game->game_h == NULL) {
        err_die(output, "Could not allocate memory for client's game header!");
    }
    memset((*client)->game->game_h, 0, sizeof(struct Game));

    // Initialize the client's game track.
    (*client)->game->track = (track_t*)malloc(sizeof(track_t));
    if ((*client)->game->track == NULL) {
        err_die(output, "Could not allocate memory for client's game track!");
    }
    memset((*client)->game->track, 0, sizeof(track_t));

    (*client)->game->track->field = (struct Field*)malloc(sizeof(struct Field));
    if ((*client)->game->track->field == NULL) {
        err_die(output, "Could not allocate memory for client's game track field!");
    }
    memset((*client)->game->track->field, 0, sizeof(struct Field));

    // Initialize the client's game track lines.
    (*client)->game->track->start_line = (struct Line*)malloc(sizeof(struct Line));
    if ((*client)->game->track->start_line == NULL) {
        err_die(output, "Could not allocate memory for client's game track start line!");
    }
    memset((*client)->game->track->start_line, 0, sizeof(struct Line));

    (*client)->game->track->main_line = (struct Line*)malloc(sizeof(struct Line));
    if ((*client)->game->track->main_line == NULL) {
        err_die(output, "Could not allocate memory for client's game track main line!");
    }
    memset((*client)->game->track->main_line, 0, sizeof(struct Line));

    // Save this client in the global clients list.
    client_count++;
    push_client(&clients_start, client);
}