#include "../include/common.h"
#include "../include/protocol.h"
#include "../include/serialization.h"
#include "../include/my_strings.h"
#include "../include/log_messages.h"
#include "../include/linked_list.h"

// 1. create link list funcs 
// 2. have global "base pointer" for games.
// 3. create a game node when CREATE GAME msg received.

// Global structures.
track_node_t *tracks_start = NULL;
static int track_count = 0;

client_node_t *clients_start = NULL;
static int client_count = 0;

game_node_t *games_start = NULL;
static int game_count = 0;

// Sockets.
int listen_socket, client_socket;

// Additional functions.
void init_fields();
void handle_sigint(int sig);

void create_game_response(char *buffer, client_t *client) {
    // Game initialization.
    game_node_t *new_game = (game_node_t*)calloc(1, sizeof(game_node_t));
    if (new_game == NULL) {
        err_die("Could not allocate memory for game!");
    }

    client->game->ID = new_game->game->ID = ++game_count;

    // Handling the message from a client.
    deserialize_msg_CG(buffer, client);
    
    log_create_game_response(stdout, client);

    // Create a token (pass) for the player.
    generate_password(client);

    // Set a response back to the client.
    bzero(buffer, MAX_BUFFER_SIZE);
    serialize_msg_CG_response(buffer, client);
}

void get_number_of_fields_response(char *buffer, client_t *client) {
    // Set a response back to the client.
    bzero(buffer, MAX_BUFFER_SIZE);
    serialize_msg_NF_response(buffer, track_count);
    printf("Sent the count of available fields to player - %s\n", from_who(client));
}

void field_info_response(char *buffer, client_t *client) {
    int chose;

    // Handling the message from a client.
    deserialize_msg_FI(buffer, &chose);
    printf("Player %s chose to get more info about field - %d\n", from_who(client), chose);
    
    // Set a response back to the client.
    bzero(buffer, MAX_BUFFER_SIZE);
    serialize_msg_FI_response(buffer, get_track(chose));
}

void list_games_response(char *buffer, client_t *client) {
    // Set a response back to the client.
    bzero(buffer, MAX_BUFFER_SIZE);
    serialize_msg_LI_response(buffer, game_count);
    printf("Sent the count of fields to player - %s\n", from_who(client));
}

void game_info_response(char *buffer, client_t *client) {
    //
}

void handle_message(char *buffer, client_t *client) {
    char msg_type[MSG_TYPE_LEN] = { buffer[0], buffer[1], '\0' };
    buffer = buffer + 3; // Pass the point of buffer after msg_type.
    printf("Received message of type - %s from player - %s\n", msg_type, from_who(client));

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
    char *buffer = (char*)calloc(sizeof(MAX_BUFFER_SIZE), 1);
    int leave_flag = 0;
    int data_n;
    client_count++;

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
        
        bzero(buffer, MAX_BUFFER_SIZE);
    }

    close(client->sock_fd);
    free(buffer);
    free(client);
    client_count--;
}

int main(int argc, char **argv) {
    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;
    client_t *client;
    socklen_t client_len;
    int err;
    int option = 1;

    signal(SIGINT, handle_sigint);

    // Define the socket settings for listening.
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family        = AF_INET;
    serv_addr.sin_addr.s_addr   = inet_addr("127.0.0.1");
    serv_addr.sin_port          = htons(PORT);

    err = setsockopt(listen_socket, SOL_SOCKET, SO_REUSEPORT | SO_REUSEADDR, (char*)&option, sizeof(option));
    if (err < 0) {
        err_die("Setting options on socket failed!");
    }

    err = bind(listen_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (err < 0) {
        err_die("Binding failed!");
    }

    err = listen(listen_socket, MAX_CLIENTS);
    if (err < 0) {
        err_die("Listening failed!");
    }

    // Server/Game has started.

    printf("Server is on...\n");

    init_fields();

    client_len = sizeof(client_addr);
    client_socket = accept(listen_socket, (struct sockaddr*)&client_addr, &client_len);
    
    client = (client_t*)calloc(1, sizeof(client_t));
    if (client == NULL) {
        err_die("Could not allocate memory for a client!");
    }
    client->address      = client_addr;
    client->sock_fd      = client_socket;
    client->player->ID   = ++client_count;
    strcpy(client->ip, ip_addr(client_addr));

    push_player(clients_start, client);
    handle_client(client);

    shutdown(listen_socket, SHUT_RDWR);
    shutdown(client_socket, SHUT_RDWR);
    printf("Goodbye!\n");
    exit(EXIT_SUCCESS);
}

void handle_sigint(int sig) {
    shutdown(listen_socket, SHUT_RDWR);
    shutdown(client_socket, SHUT_RDWR);
    printf("-Caught signal - %d\n", sig);
    exit(EXIT_FAILURE);
}

void init_fields() {
    // Initialize a track.
    track_t *track = (track_t*)calloc(1, sizeof(track_t));
    if (track == NULL) {
        err_die("Could not allocate memory for a track!");
    }

    // Initialize the track's field.
    track->field = (struct Field*)calloc(1, sizeof(struct Field));
    if (track == NULL) {
        err_die("Could not allocate memory for a field!");
    }

    track->field->ID = ++track_count;
    memset(track->field->name, 0, FIELD_NAME_LEN);
    strcpy(track->field->name, "Grand Tour");
    track->field->Width = 46;
    track->field->Height = 12;

    // Initialize the track's start line.
    track->start_line = (struct Line*)calloc(1, sizeof(struct Line));
    if (track->start_line == NULL) {
        err_die("Could not allocate memory for a field's start line!");
    }

    track->start_line->beggining.x = 15.1;
    track->start_line->beggining.y = 2.1;
    track->start_line->end.x = 15.1;
    track->start_line->end.y = 5.1;

    // Initialize the track's main line.
    track->main_line = (struct Line*)calloc(1, sizeof(struct Line));
    if (track->main_line == NULL) {
        err_die("Could not allocate memory for a field's main line!");
    }

    track->main_line->beggining.x = 10.1;
    track->main_line->beggining.y = 5.1;
    track->main_line->end.x = 25.1;
    track->main_line->end.y = 6.1;

    // Initialize the track's extra lines.
    track->n_extra_lines = 0;

    // Save this field in the global fields array.
    // fields[track_count-1] = track; 
}