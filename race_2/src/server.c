#include "../include/common.h"
#include "../include/protocol.h"
#include "../include/serialization.h"
#include "../include/log_messages.h"
#include "../include/linked_list.h"

// TODO:
// -WARNING: Meanwhile, in case of undefined communciation from client side,
// server dies. 
// Should be handled by sending an error message (type/code).
// back to client for it to handle itself.
// -NOTE:! In the protocol it is stated that MSG_TYPE will always be sent !!!
// Will have to change the starting points and modify serialization funcs. 
// -NOTE: Before message is to be deserialized, call check_err_type()
// which would ONLY READ the first bytes (or a error msg) to detect
// whether it should so [something].
// -NOTE: Comment code thoroughly.

// LLs and variables of the main game types.
client_node_t *clients_start = NULL;
unsigned int client_count    = 0;
int client_id                = 0;

game_node_t *games_start     = NULL;
unsigned int game_count      = 0;
int game_id                  = 0;

track_node_t *tracks_start   = NULL;
unsigned int track_count     = 0;
int track_id                 = 0;

// The server's socket.
int listen_socket;

// Variables for global flag storing.
FILE *output;
char *ip;
int  port;

// Helper function prototypes.
void handle_sigint(int sig);
void err_die_server(FILE *fp, char* err_msg);
FILE *change_log_output(FILE *fp, char *path);

// Global structure initialization prototypes.
void init_client(client_t **client);
void init_game(game_t **game, int chosen_field_id);
void init_tracks();;

void create_game_response(char *buffer, client_t *client) {
    game_t  *game;
    char    *client_name, *game_name;
    int     chosen_field_id;

    // Allocate and initialiase the names.
    client_name = malloc(CLIENT_NAME_LEN);
    if (client_name == NULL) {
        err_die_server(output, "Could not allocate memory for client name!");
    }

    game_name = malloc(GAME_NAME_LEN);
    if (game_name == NULL) {
        err_die_server(output, "Could not allocate memory for game name!");
    }

    memset(client_name, '\0', CLIENT_NAME_LEN);
    memset(game_name,   '\0', GAME_NAME_LEN);

    // Handling the message from a client.
    deserialize_msg_CG(buffer, client_name, game_name, &chosen_field_id);

    // Create a new game.
    init_game(&game, chosen_field_id);

    // Copy the names to the corresponding fields.
    strcpy(game->game_h->name, game_name);
    strcpy(client->player->name, client_name);

    // Assign the new game for the client.
    client->game = game;

    // Create a password for the client.
    generate_password(client);

    // Set a response back to the client.
    memset(buffer, 0, BUF_SIZE_WO_TYPE);
    serialize_msg_CG_response(buffer, client); // + msg_type

    free(client_name);
    free(game_name);

    log_create_game_response(output, client);
}

void get_number_of_fields_response(char *buffer, client_t *client) {
    // Set a response back to the client.
    memset(buffer, '\0', BUF_SIZE_WO_TYPE);
    serialize_msg_NF_response(buffer, track_count);

    log_get_number_of_fields_response(output, client);
}

void field_info_response(char *buffer, client_t *client) {
    track_t *chosen_track;
    int chose;

    // Handling the message from a client.
    deserialize_msg_FI(buffer, &chose);

    chosen_track = get_track_by_id(&tracks_start, chose);
    if(chosen_track == NULL) {
        err_die_server(output, "Could not find the requested field!");
    }
    
    // Set a response back to the client.
    memset(buffer, '\0', BUF_SIZE_WO_TYPE);
    serialize_msg_FI_response(buffer, chosen_track);

    log_field_info_response(output, client, chose);
}

void list_games_response(char *buffer, client_t *client) {
    // Set a response back to the client.
    memset(buffer, '\0', BUF_SIZE_WO_TYPE);
    serialize_msg_LI_response(buffer, game_count);

    log_list_games_response(output, client);
}

void game_info_response(char *buffer, client_t *client) {
    //
}

void handle_message(char *buffer, client_t *client) {
    char msg_type[MSG_TYPE_LEN] = { buffer[0], buffer[1], '\0' };
    char err_msg[ERR_MSG_LEN];

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
        // NOTE: Have a function - print_log AND send_err_msg() in one func.
        sprintf(err_msg, "Received an unknown type - %s", msg_type);
        err_die_server(output, err_msg);
    }
}

void handle_client(client_t *client) {
    char    *buffer;
    char    err_msg[ERR_MSG_LEN];
    int     leave_flag, data_n, del_ret;

    leave_flag = 0;

    // Allocate and initialize buffer (one buffer per client).
    buffer = malloc(MAX_BUFFER_SIZE);
    if (buffer == NULL) {
        err_die_server(output, "Could not allocate memory for a buffer!");
    }
    
    memset(buffer, '\0', MAX_BUFFER_SIZE);

    while(1) {
        if (leave_flag) {
            break;
        }

        data_n = recv(client->sock_fd, buffer, MAX_BUFFER_SIZE, 0);
        if (data_n > 0) {
            if (strlen(buffer) > 0) {
                handle_message(buffer, client);
                // NOTE:? Should I send exact number of bytes (return size from
                // handle_message() sequentially)
                // EXP: Passing buffer + 3 in order to NOT send the msg_type.
                data_n = send(client->sock_fd, buffer, MAX_BUFFER_SIZE, 0);
                if (data_n < 0) {
                    err_die_server(output, "An error occurred sending a message!");
                }
            }
        } else if (data_n == 0) {
            log_lost_connection(output, client);
            leave_flag = 1;
        } else {
            err_die_server(output, "An error occurred receiving a message!");
        }

        memset(buffer, '\0', MAX_BUFFER_SIZE);
    }

    // Stop handling the client gracefully.
    free(buffer);
    client_count--;

    del_ret = remove_by_client_id(output, &clients_start, client->player->ID);
    if (del_ret != RGOOD) {
        sprintf(err_msg, "Could not delete client by ID. ERROR: %d", del_ret);
        err_die_server(output, err_msg);
    }
}

int main(int argc, char **argv) {
    struct sockaddr_in  serv_addr, client_addr;
    socklen_t           client_addr_len;
    int                 err, option, flag;

    client_t *client;
    track_t *track;

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
                output = change_log_output(output, optarg);
                break;
            case 'h':
                help();
                exit(EXIT_SUCCESS);
            default:
                help();
                exit(EXIT_FAILURE);
        }
    }

    // Define various socket settings for listening.
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family        = AF_INET;
    serv_addr.sin_addr.s_addr   = inet_addr(ip); // NOTE:? Should it be INADDR_ANY
    serv_addr.sin_port          = htons(port);

    client_addr_len = sizeof(client_addr);

    option = 1;
    err = setsockopt(listen_socket, SOL_SOCKET, SO_REUSEPORT | SO_REUSEADDR, (char*)&option, sizeof(option));
    if (err < 0) {
        err_die_server(output, "Setting options on socket failed!");
    }

    err = bind(listen_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (err < 0) {
        err_die_server(output, "Binding failed!");
    }

    err = listen(listen_socket, MAX_CLIENTS);
    if (err < 0) {
        err_die_server(output, "Listening failed!");
    }

    // === Server/Game has started === //

    fprintf(output, "%sServer is on...%s\n", ANSI_BLINK, ANSI_RESET_ALL);

    // Initialize all tracks.
    init_tracks(&track);

    // Initialize client and define various client fields.
    init_client(&client);
    client->sock_fd = accept(listen_socket, (struct sockaddr*)&client_addr, &client_addr_len);
    client->address = client_addr;
    strcpy(client->ip, ip_addr(client_addr));

    // NOTE: Manage in a thread.
    handle_client(client);

    /* Exit program gracefully */

    shutdown(listen_socket, SHUT_RDWR);
    close(listen_socket);

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
    sprintf(err_msg, "Caught signal - %d", sig);
    
    err_die_server(output, err_msg);
}

void err_die_server(FILE *fp, char* err_msg) {
    // General message handling.
    log_time_header(fp);
    fprintf(fp, "%sERROR: %s ", ANSI_RED, err_msg);
    fprintf(fp, "(errno = %d): %s%s\n", errno, strerror(errno), ANSI_RESET_ALL);

    // Shutdown and close the socket of the server.
    shutdown(listen_socket, SHUT_RDWR);
    close(listen_socket);

    // Freeing memory.
    remove_all_clients(output, &clients_start);
    remove_all_tracks(output, &tracks_start);
    remove_all_games(output, &games_start);

    exit(EXIT_FAILURE);
}

FILE *change_log_output(FILE *fp, char *path) {
    fp = fopen(path, "w");
    if (fp == NULL) {
        err_die_server(stdout, "Error opening the given log file!");
    }

    return fp;
}

void init_client(client_t **client) {
    // Client struct allocation.
    *client = malloc(sizeof(client_t));
    if (*client == NULL) {
        err_die_server(output, "Could not allocate memory for a client!");
    }

    // Client IP address allocation.
    (*client)->ip = malloc(IP_LEN);
    if ((*client)->ip == NULL) {
        err_die_server(output, "Could not allocate memory for client's IP!");
    }

    // Client password allocation.
    (*client)->password = malloc(CLIENT_PASS_LEN);
    if ((*client)->password == NULL) {
        err_die_server(output, "Could not allocate memory for client's password!");
    }

    // Client player info struct allocation.
    (*client)->player = malloc(sizeof(struct Player_info));
    if ((*client)->player == NULL) {
        err_die_server(output, "Could not allocate memory for client's player!");
    }

    // Initialising the char arrays.
    memset((*client)->ip, '\0', IP_LEN);
    memset((*client)->player->name, '\0', CLIENT_NAME_LEN);

    // Allocated field definitions.
    (*client)->player->ID           = ++client_id;
    (*client)->player->angle        = 0.0f;
    (*client)->player->speed        = 0.0f;
    (*client)->player->acceleration = 0.0f;
    (*client)->player->laps         = 0;
    (*client)->player->position.x   = 0;
    (*client)->player->position.y   = 0;

    // Increase the count of players on the server.
    client_count++;

    // Save this client in the global clients list.
    push_client(&clients_start, client);
}

void init_game(game_t **game, int chosen_field_id) {
    track_t *chosen_track;

    // Game struct allocation.
    *game = malloc(sizeof(game_t));
    if (*game == NULL) {
        err_die_server(output, "Could not allocate memory for game!");
    }

    // Game header allocation.
    (*game)->game_h = malloc(sizeof(struct Game));
    if ((*game)->game_h == NULL) {
        err_die_server(output, "Could not allocate memory for game!");
    }

    // Initialising the char arrays.
    memset((*game)->game_h->name, '\0', GAME_NAME_LEN);

     // Allocated field definitions.
    (*game)->ID                        = ++game_id;
    (*game)->game_h->status            = 0;
    (*game)->game_h->WinnerPlayerID    = 0;

    // Assigning a field to the game.
    chosen_track = get_track_by_id(&tracks_start, chosen_field_id);
    if (chosen_track == NULL) {
        err_die_server(output, "Could not find the requested field!");
    }

    (*game)->track = chosen_track;

    // Increase the count of games on the server.
    game_count++;

    // Save this game in the global games list.
    push_game(&games_start, game);
}

void init_tracks(track_t **track) {
    // Track struct allocation.
    *track = malloc(sizeof(track_t));
    if (track == NULL) {
        err_die_server(output, "Could not allocate memory for a track!");
    }

    // Track field allocation.
    (*track)->field = malloc(sizeof(struct Field));
    if (track == NULL) {
        err_die_server(output, "Could not allocate memory for a field!");
    }

    // Track start line allocation.
    (*track)->start_line = malloc(sizeof(struct Line));
    if ((*track)->start_line == NULL) {
        err_die_server(output, "Could not allocate memory for a field's start line!");
    }

    // Tracks main line allocation.
    (*track)->main_line = malloc(sizeof(struct Line));
    if ((*track)->main_line == NULL) {
        err_die_server(output, "Could not allocate memory for a field's main line!");
    }

    // Initialising the char arrays.
    memset((*track)->field->name, '\0', FIELD_NAME_LEN);

    // Allocated field definitions.
    (*track)->n_extra_lines            = 0;

    (*track)->field->ID                = ++track_id;
    (*track)->field->Width             = 46;
    (*track)->field->Height            = 12;
    strcpy((*track)->field->name, "Grand Tour");

    (*track)->start_line->beggining.x  = 15.1;
    (*track)->start_line->beggining.y  = 2.1;
    (*track)->start_line->end.x        = 15.1;
    (*track)->start_line->end.y        = 5.1;

    (*track)->main_line->beggining.x   = 10.1;
    (*track)->main_line->beggining.y   = 5.1;
    (*track)->main_line->end.x         = 25.1;
    (*track)->main_line->end.y         = 6.1;

    // Increase the count of tracks on the server.
    track_count++;

    // Save this track in the global tracks list.
    push_track(&tracks_start, track);
}