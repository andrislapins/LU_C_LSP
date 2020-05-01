#include "../include/common.h"
#include "../include/protocol.h"
#include "../include/serialization.h"
#include "../include/log_messages.h"
#include "../include/linked_list.h"

// NOTE: have default values like IP, port, log_output but add
// store them here globally + help flags/options.
// For log_ouput I should have a seperate func to create/overwrite log file.
// Use the special flag handling library!

// Global structures.
track_node_t *tracks_start = NULL;
static unsigned int track_count = 0;
static int track_id = 0;

client_node_t *clients_start = NULL;
static unsigned int client_count = 0;
static int client_id = 0;

game_node_t *games_start = NULL;
static unsigned int game_count = 0;
static int game_id = 0;

// Sockets.
int listen_socket, client_socket;

// Default flag values.
FILE *output;
char *ip;
int port;

// Helper function prototypes.
void handle_sigint(int sig);
void change_log_output(FILE *fp, char *path);
void help();

// Global structure initialization prototypes.
void init_tracks_once();
game_t *init_game(int field);
client_t *init_client(int client_socket, struct sockaddr_in client_addr);

void create_game_response(char *buffer, client_t *client) {
    game_t *game;

    // Handling the message from a client.
    deserialize_msg_CG(buffer, client);

    // Create a new game.
    game = init_game(client->game->track->field->ID);

    // Assign the new game to the client, and create a password for the client.
    client->game = game;
    generate_password(client);

    // Set a response back to the client.
    bzero(buffer, MAX_BUFFER_SIZE);
    serialize_msg_CG_response(buffer, client);

    log_create_game_response(stdout, client);
}

void get_number_of_fields_response(char *buffer, client_t *client) {
    // Set a response back to the client.
    bzero(buffer, MAX_BUFFER_SIZE);
    serialize_msg_NF_response(buffer, track_count);

    log_get_number_of_fields_response(stdout, client);
}

void field_info_response(char *buffer, client_t *client) {
    int chose;

    // Handling the message from a client.
    deserialize_msg_FI(buffer, &chose);
    
    // Set a response back to the client.
    bzero(buffer, MAX_BUFFER_SIZE);
    serialize_msg_FI_response(buffer, get_track_by_id(tracks_start, chose));

    log_field_info_response(stdout, client, &chose);
}

void list_games_response(char *buffer, client_t *client) {
    // Set a response back to the client.
    bzero(buffer, MAX_BUFFER_SIZE);
    serialize_msg_LI_response(buffer, game_count);

    log_list_games_response(stdout, client);
}

void game_info_response(char *buffer, client_t *client) {
    //
}

void handle_message(char *buffer, client_t *client) {
    char msg_type[MSG_TYPE_LEN] = { buffer[0], buffer[1], '\0' };
    buffer = buffer + 3; // Pass the point of buffer after msg_type.
    log_recvd_msg_type(stdout, client, msg_type);

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
    char *buffer = (char*)malloc(MAX_BUFFER_SIZE * sizeof(*buffer));
    int leave_flag = 0;
    int data_n;
    client_count++;

    while(1) {
        if (leave_flag) {
            break;
        }

        bzero(buffer, MAX_BUFFER_SIZE);

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
        err_die(stdout, "Setting options on socket failed!");
    }

    err = bind(listen_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (err < 0) {
        err_die(stdout, "Binding failed!");
    }

    err = listen(listen_socket, MAX_CLIENTS);
    if (err < 0) {
        err_die(stdout, "Listening failed!");
    }

    // === Server/Game has started === //

    printf("Server is on...\n");

    init_tracks_once();

    // Initiliaze various client values.
    client_addr_len = sizeof(client_addr);
    client_socket   = accept(listen_socket, (struct sockaddr*)&client_addr, &client_addr_len);
    client          = init_client(client_socket, client_addr);

    // NOTE: Manage in a thread.
    handle_client(client);

    shutdown(listen_socket, SHUT_RDWR);
    shutdown(client_socket, SHUT_RDWR);
    printf("Goodbye!\n");
    exit(EXIT_SUCCESS);
}

/* === Non-communication specific/ Helper functions === */

void handle_sigint(int sig) {
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
    // Initialize a track.
    track_t *track = (track_t*)calloc(1, sizeof(track_t));
    if (track == NULL) {
        err_die(stdout, "Could not allocate memory for a track!");
    }

    // Initialize the track's field.
    track->field = (struct Field*)calloc(1, sizeof(struct Field));
    if (track == NULL) {
        err_die(stdout, "Could not allocate memory for a field!");
    }

    track->field->ID        = ++track_id; // Increment track ID.
    track->field->Width     = 46;
    track->field->Height    = 12;
    memset(track->field->name, 0, FIELD_NAME_LEN);
    strcpy(track->field->name, "Grand Tour");

    // Initialize the track's start line.
    track->start_line = (struct Line*)calloc(1, sizeof(struct Line));
    if (track->start_line == NULL) {
        err_die(stdout, "Could not allocate memory for a field's start line!");
    }

    track->start_line->beggining.x  = 15.1;
    track->start_line->beggining.y  = 2.1;
    track->start_line->end.x        = 15.1;
    track->start_line->end.y        = 5.1;

    // Initialize the track's main line.
    track->main_line = (struct Line*)calloc(1, sizeof(struct Line));
    if (track->main_line == NULL) {
        err_die(stdout, "Could not allocate memory for a field's main line!");
    }

    track->main_line->beggining.x   = 10.1;
    track->main_line->beggining.y   = 5.1;
    track->main_line->end.x         = 25.1;
    track->main_line->end.y         = 6.1;

    // Initialize the track's extra lines.
    track->n_extra_lines            = 0;

    // Save this track in the global tracks list.
    push_track(tracks_start, track);
}

game_t *init_game(int chosen_field_id) {
    game_t *new_game;
    
    // Initialize the game.
    new_game = (game_t*)calloc(1, sizeof(game_t));
    if (new_game == NULL) {
        err_die(stdout, "Could not allocate memory for game!");
    }

    new_game->ID                        = ++game_id;
    new_game->game_h->status            = 0;
    new_game->game_h->WinnerPlayerID    = 0;
    memset(new_game->game_h->name, 0, GAME_NAME_LEN);

    // Save the chosen track for the new game.
    new_game->track = get_track_by_id(tracks_start, chosen_field_id);

    // Save this game in the global games list.
    push_game(games_start, new_game);
    return new_game;
}

client_t *init_client(int client_socket, struct sockaddr_in client_addr) {
    // Initialize the client.
    client_t *client = (client_t*)malloc(sizeof(client_t) * sizeof(*client));
    if (client == NULL) {
        err_die(stdout, "Could not allocate memory for a client!");
    }

    // Initialize the client's socket values.
    client->address      = client_addr;
    client->sock_fd      = client_socket;
    strcpy(client->ip, ip_addr(client_addr));

    // Initialize the client's player struct.
    client->player = (struct Player_info*)malloc(sizeof(struct Player_info));
    if (client->player == NULL) {
        err_die(stdout, "Could not allocate memory for client's player!");
    }

    client->player->ID   = ++client_id; 

    // Initialize the client's game struct.
    client->game = (game_t*)malloc(sizeof(game_t));
    if (client->game == NULL) {
        err_die(stdout, "Could not allocate memory for client's game!");
    }

    client->game->game_h = (struct Game*)malloc(sizeof(struct Game));
    if (client->game->game_h == NULL) {
        err_die(stdout, "Could not allocate memory for client's game header!");
    }

    // Initialize the client's game track.
    client->game->track = (track_t*)malloc(sizeof(track_t));
    if (client->game->track == NULL) {
        err_die(stdout, "Could not allocate memory for client's game track!");
    }

    client->game->track->field = (struct Field*)malloc(sizeof(struct Field));
    if (client->game->track->field == NULL) {
        err_die(stdout, "Could not allocate memory for client's game track field!");
    }

    // Initialize the client's game track lines.
    client->game->track->start_line = (struct Line*)malloc(sizeof(struct Line));
    if (client->game->track->start_line == NULL) {
        err_die(stdout, "Could not allocate memory for client's game track start line!");
    }

    client->game->track->main_line = (struct Line*)malloc(sizeof(struct Line));
    if (client->game->track->main_line == NULL) {
        err_die(stdout, "Could not allocate memory for client's game track main line!");
    }

    // Save this client in the global clients list.
    push_client(clients_start, client);
    return client;
}