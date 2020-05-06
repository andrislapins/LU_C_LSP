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
// -NOTE: On the client, before message is to be deserialized, call check_err_type()
// which would ONLY READ the first bytes (or a error msg) to detect
// whether it should do [something].
// -NOTE: Comment code thoroughly.

// Mutex initializer.
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

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
int  port;

// Helper function prototypes.
void handle_sigint(int sig);
void err_die_server(FILE *fp, char* err_msg);
FILE *change_log_output(FILE *fp, char *path);

// Global structure initialization prototypes.
void init_client(client_t **client, int client_sock);
void init_game(game_t **game, int chosen_field_id);
void init_tracks();

void gameplay(
    char *buffer, client_t *client, 
    int g_client_count, client_t *g_clients[MAX_CLIENTS_PER_GAME]
) {
    char                msg_type[MSG_TYPE_LEN];
    // NOTE: client_t *t client arg - is being used as a "placeholder"
    // for overwriting incoming player data.

    int client_socket[MAX_CLIENTS_PER_GAME]; // NOTE:? Or MAX_CLIENTS_PER_GAME.
    int master_socket;
    int new_socket;
    int activity;
    int opt = TRUE;
    int addrlen; 
    int valread;
    int max_sd;
    int sd;
    int ret;
    int i;

    struct sockaddr_in address;
    fd_set readfds;

    // Set up the sets and connections.    
    for (i = 0; i < MAX_CLIENTS_PER_GAME; i++) {
        client_socket[i] = 0;
    }

    master_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (master_socket == 0) {
        err_die_server(output, "UDP socket failed!");
    }

    ret = setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
    if (ret < 0) {
        err_die_server(output, "Setsockopt failed!");
    }

    address.sin_family        = AF_INET;
    address.sin_addr.s_addr   = htonl(INADDR_ANY);
    address.sin_port          = htons(port+1);

    ret = bind(master_socket, (const struct sockaddr*)&address, sizeof(address));
    if (ret < 0) {
        err_die_server(output, "Could not bind UDP socket!");
    }

    // Max of 3 pending connection.
    ret = listen(master_socket, 4);
    if (ret < 0) {
        err_die_server(output, "Listening UDP failed!");
    }

    addrlen = sizeof(address);

    printf("\nGAMEPLAY\n\n");

    game_id = client->game->ID;

    /* UPDATE PLAYER */

    strcpy(msg_type, "UP\0");

    struct timeval tv;

    // Manage the connections in a loop.
    do {
        tv.tv_sec = 5; 
        tv.tv_usec = 0;

        // Clear the socket set.
        FD_ZERO(&readfds);

        // Add master socket to set.
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        // Add child to socket set.
        for (i = 0; i < MAX_CLIENTS_PER_GAME; i++) {
            // Socket descriptor.
            sd = client_socket[i];

            if (sd > 0) {
                FD_SET(sd, &readfds);
            }

            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        // Wait for an acitivity on one of the sockets, timeout is NULL -
        // so wait indefinintely.
        activity = select(max_sd+1, &readfds, NULL, NULL, &tv);

        if ((activity < 0) && (errno!=EINTR)) {
            fprintf(output, "Select() failed!");
        }

        // If something happened on the master socket, then its an incoming
        // connection.
        if (FD_ISSET(master_socket, &readfds)) {
            new_socket = accept(master_socket, (struct sockaddr*)&address, (socklen_t*)&addrlen);
            if (new_socket < 0) {
                err_die_server(output, "UDP accept failed!");
            }

            // Display of socket number - used in send and receive commands  
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n",
                    new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // SENDING INITIAL MESSAGE: send all player data maybe.
            ret = send(new_socket, buffer, MAX_BUFFER_SIZE, 0);
            if (ret) {
                fprintf(output, "Return value from send() = %d\n", ret);
                err_die_server(output, "Failed to send message!");
            }

            // Add new socket to array of sockets.
            for (i = 0; i < MAX_CLIENTS_PER_GAME; i++) {
                // If position is empty.
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    fprintf(output, "Adding to list of sockets as %d\n", i);

                    break;
                }
            }
        }

        // Else its some IO operation on some other socket.
        for (i = 0; i < MAX_CLIENTS_PER_GAME; i++) {
            sd = client_socket[i];

            if (FD_ISSET(sd, &readfds)) {
                // Check if it was for closing, and also read the incoming message.
                if ((valread = read(sd, buffer, MAX_BUFFER_SIZE)) == 0) {
                    getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                    fprintf(
                        output, "Client disconnected, ip %s, port %d\n",
                        inet_ntoa(address.sin_addr), ntohs(address.sin_port)
                    );

                    close(sd);
                    client_socket[i] = 0;
                }
            }
            // Send a message
            else
            {
                // Deserialization and serialization of player data.
                deserialize_msg_UP(buffer, msg_type, client);

                // Overwrite data of the received client/players.

                memset(buffer, '\0', MAX_BUFFER_SIZE);

                serialize_msg_UP_response(
                    buffer, msg_type, output, game_id, g_client_count, g_clients
                );

                printf("loop inside\n");
                send(sd, buffer, MAX_BUFFER_SIZE, 0);
            }
        }
        printf("loop outside\n");
    } while (1);

    // END all the connection of the players of this game.
}

// start_game_response send info about the started game to every client of the game.
void start_game_response(char *buffer, client_t *client) {
    client_t        *g_clients[MAX_CLIENTS_PER_GAME];
    char            msg_type[MSG_TYPE_LEN];
    char            send_err_msg[ERR_MSG_LEN];
    char            *password;
    client_node_t   *current;
    ssize_t         data_n;
    int             game_id;
    int             g_client_count;
    int             i;

    // Save received password in temporal space.
    password = malloc(CLIENT_PASS_LEN);
    if (password == NULL) {
        err_die_server(output, "Could not allocate memory for password when starting game!");
    }

    memset(password, '\0', CLIENT_PASS_LEN);

    deserialize_msg_SG(buffer, &game_id, password);

    if (strcmp(client->password, password) != 0) {
        err_die_server(output, "Received incorrect client password!");
    }

    strcpy(msg_type, "SG\0");

    // Collect the players info (client_t) of the chosen game.
    current = clients_start;
    g_client_count = 0;

    for(i = 0; current != NULL && g_client_count < MAX_CLIENTS_PER_GAME; i++) {
        if(current->client->game->ID == game_id) {
            g_clients[g_client_count] = current->client;
            g_client_count++;
        }

        current = current->next_client;
    }

    /* Initialize the values to create the SG msg */

    // Start from the begging (overwriting msg type too).
    buffer = buffer - 3;

    memset(buffer, '\0', MAX_BUFFER_SIZE);

    // Create the SG msg to send all the clients of the game.
    serialize_msg_SG_response(
        buffer, msg_type, g_client_count, g_clients, client->game
    );

    // Send the START GAME (info about all the players of the game)
    // to all players of the game.    
    for (i = 0; i < g_client_count; i++) {
        if (g_clients[i]->game->ID == client->game->ID) {
            data_n = send(g_clients[i]->sock_fd, buffer, MAX_BUFFER_SIZE, 0);
            if (data_n < 0) {
                sprintf(send_err_msg, "Could not send message of type %s!", msg_type);
                err_die_server(output, send_err_msg);
            }

            log_msg_SG_sent(output, msg_type, g_clients[i]->player->ID, 
                g_clients[i]->player->name, client
            );
        }
    }

    free(password);

    // Traverse to the function which handles the gameplay.
    gameplay(buffer, client, g_client_count, g_clients);
}

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

    pthread_mutex_lock(&clients_mutex);
    // Copy the names to the corresponding fields.
    strcpy(game->game_h->name, game_name);
    strcpy(client->player->name, client_name);

    // CHECK and FREE the dummy game and only then assign the new game 
    // for the client. NOTE: Should I allow a client create two games?
    if (client->game->ID == 0) {
        free(client->game);
    }
    
    client->game = game;

    pthread_mutex_unlock(&clients_mutex);

    // Free these names because the strings have been copied.
    free(client_name);
    free(game_name);

    // Create a password for the client.
    generate_password(output, client);

    // Set a response back to the client.
    memset(buffer, 0, BUF_SIZE_WO_TYPE);
    serialize_msg_CG_response(buffer, client);

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
    int     *gid_arr, ret;
    char    err_msg[ERR_MSG_LEN];

    // Get all the game IDs.
    ret = get_game_ids(output, &games_start, &gid_arr, game_count);
    if (ret != RGOOD) {
        sprintf(err_msg, "Could not get game IDs. ERROR: %d", ret);
        err_die_server(output, err_msg);
    }

    // Set a response back to the client.
    memset(buffer, '\0', BUF_SIZE_WO_TYPE);
    serialize_msg_LI_response(buffer, game_count, &gid_arr);

    free(gid_arr);

    log_list_games_response(output, client);
}

void game_info_response(char *buffer, client_t *client) {
    client_t *g_clients[MAX_CLIENTS_PER_GAME];
    client_node_t *current;
    game_t *chosen_game;
    int chosen_gid, g_client_count;

    // Handling the message from a client.
    deserialize_msg_GI(buffer, &chosen_gid);

    // Get the chosen game instance.
    chosen_game = get_game_by_id(&games_start, chosen_gid);
    if(chosen_game == NULL) {
        err_die_server(output, "Could not find the requested game!");
    }

    // Collect the players (client_t) of the chosen game.
    current = clients_start;
    g_client_count = 0;

    // OPTION: compare on adding "|| g_client_count < MAX_CLIENTS_PER_GAME".
    for(int i = 0; current != NULL; i++) {
        if(current->client->game->ID == chosen_gid) {
            g_clients[g_client_count] = current->client;
            g_client_count++;
        }

        current = current->next_client;
    }

    // CHECK and FREE the dummy game and only then assign the new game 
    // for the client. NOTE: Should I allow a client create two games?
    pthread_mutex_lock(&clients_mutex);

    if (client->game->ID == 0) {
        free(client->game);
    }
    
    client->game = chosen_game;

    pthread_mutex_unlock(&clients_mutex);

    // Set a response back to the client.
    memset(buffer, '\0', BUF_SIZE_WO_TYPE);
    serialize_msg_GI_response(buffer, chosen_game, g_client_count, g_clients);

    log_game_info_response(output, client, chosen_gid);
}

void join_game_response(char *buffer, client_t *client) {
    game_t      *chosen_game;
    int         chosen_game_id;
    char        *client_name;
    ssize_t     data_n;
    char        msg_type[MSG_TYPE_LEN];

    strcpy(msg_type, "JG\0");

    // Allocate and initialiase the names.
    client_name = malloc(CLIENT_NAME_LEN);
    if (client_name == NULL) {
        err_die_server(output, "Could not allocate memory for client name!");
    }

    memset(client_name, '\0', CLIENT_NAME_LEN);

    // Handling the message from a client.
    deserialize_msg_JG(buffer, &chosen_game_id, client_name);

    // Find and assign the new game for the client.
    chosen_game = get_game_by_id(&games_start, chosen_game_id);
    if(chosen_game == NULL) {
        err_die_server(output, "Could not find the requested game!");
    }

    pthread_mutex_lock(&clients_mutex);

    client->game = chosen_game;

    pthread_mutex_unlock(&clients_mutex);

    // Copy the names to the corresponding fields.
    strcpy(client->player->name, client_name);

    // Create a password for the client.
    generate_password(output, client);

    // Set a response back to the client.
    buffer = buffer - 3;
    memset(buffer, '\0', BUF_SIZE_WO_TYPE);
    strcpy(buffer, msg_type);
    serialize_msg_JG_response(buffer+3, client);

    data_n = send(client->sock_fd, buffer, MAX_BUFFER_SIZE, 0);
    if (data_n < 0) {
        err_die_server(output, "Could not send message of type JG!");
    }

    free(client_name);

    // Log the response.
    log_join_game_response(output, client);

    /* Notify other players */

    char            send_err_msg[ERR_MSG_LEN];
    client_node_t   *current;

    current = clients_start;
    strcpy(msg_type, "NF\0");

    while (current != NULL) {
        memset(buffer, '\0', MAX_BUFFER_SIZE);

        // Do NOT send this type of msg for the client himself. As I understood
        // its gonna be sent in handle_client in any case back to client.
        // NOTE: Or send it?
        if (current->client->player->ID == client->player->ID) {
            current = current->next_client;
            continue;
        }

        if (current->client->game->ID == chosen_game->ID) {
            serialize_msg_NOTIFY(
                buffer, msg_type, client->player->ID, client->player->name
            );

            data_n = send(current->client->sock_fd, buffer, MAX_BUFFER_SIZE, 0);
            if (data_n < 0) {
                sprintf(send_err_msg, "Could not send message of type %s!", msg_type);
                err_die_server(output, send_err_msg);
            }

            log_msg_NOTIFY_sent(
                output, msg_type, current->client->player->ID, 
                current->client->player->name, client
            );
        }

        current = current->next_client;
    }
}

void handle_message(char *buffer, client_t *client) {
    char msg_type[MSG_TYPE_LEN] = { buffer[0], buffer[1], '\0' };

    // Pass the point of buffer after msg_type.
    buffer = buffer + 3;

    if (strcmp(msg_type, "CG") == 0) { // CREATE GAME
        log_recvd_msg_type(output, client, msg_type);

        create_game_response(buffer, client);
    } else if (strcmp(msg_type, "NF") == 0) { // GET NUMBER OF FIELDS
        log_recvd_msg_type(output, client, msg_type);

        get_number_of_fields_response(buffer, client);
    } else if (strcmp(msg_type, "FI") == 0) { // FIELD INFO
        log_recvd_msg_type(output, client, msg_type);

        field_info_response(buffer, client);
    } else if (strcmp(msg_type, "LI") == 0) { // LIST GAMES
        log_recvd_msg_type(output, client, msg_type);

        list_games_response(buffer, client);
    } else if (strcmp(msg_type, "GI") == 0) { // GAME INFO
        log_recvd_msg_type(output, client, msg_type);

        game_info_response(buffer, client);
    } else if (strcmp(msg_type, "JG") == 0) { // JOIN GAME
        log_recvd_msg_type(output, client, msg_type);

        join_game_response(buffer, client);
    } else if (strcmp(msg_type, "SG") == 0) { // START GAME 
        log_recvd_msg_type(output, client, msg_type);

        start_game_response(buffer, client);
    } else if (strcmp(msg_type, "PI") == 0) { // PING PONG
        buffer = buffer - 3;
        strcpy(buffer, "PI\0");
    } else {
        // NOTE: Have a function - print_log AND send_err_msg() in one func.
        // sprintf(err_msg, "Received an unknown type - %s", msg_type);
        // err_die_server(output, err_msg);
    }
}

void *handle_client(void *arg) {
    char    *buffer;
    char    err_msg[ERR_MSG_LEN];
    int     leave_flag, data_n, del_ret;

    leave_flag = 0;
    client_t *client = (client_t*)arg;

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
                // Check the msg type and handle accordingly.
                handle_message(buffer, client);
                // Send the requested data to the client.
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

        // Reinitialise the message buffer.
        memset(buffer, '\0', MAX_BUFFER_SIZE);
    }

    /* Stop handling the client gracefully */

    free(buffer);

    // Remove the client from the global LL and check for errors.
    pthread_mutex_lock(&clients_mutex);
    del_ret = remove_by_client_id(output, &clients_start, client->player->ID);
    if (del_ret != RGOOD) {
        sprintf(err_msg, "Could not delete client by ID. ERROR: %d", del_ret);
        err_die_server(output, err_msg);
    }

    client_count--;

    pthread_mutex_unlock(&clients_mutex);

    // End thread instantlly.
    pthread_detach(pthread_self());

    return NULL;
}

int main(int argc, char **argv) {
    struct sockaddr_in  serv_addr, client_addr;
    socklen_t           client_addr_len;
    int                 err, option, flag, received_conn;
    pthread_t tid;

    client_t *client;
    track_t *track;

    signal(SIGINT, handle_sigint);

    // Default flag values.
    output = stdout;
    // ip     = "127.0.0.1";
    port   = PORT;

    while ((flag = getopt(argc, argv, "p:l:h")) != -1) {
        switch (flag) {
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
    serv_addr.sin_addr.s_addr   = INADDR_ANY; 
    serv_addr.sin_port          = htons(port);

    // client_addr_len = sizeof(client_addr);

    option = 1;
    err = setsockopt(listen_socket, SOL_SOCKET, SO_REUSEPORT | SO_REUSEADDR, (char*)&option, sizeof(option));
    if (err < 0) {
        err_die_server(output, "Setting options on socket failed!");
    }

    err = bind(listen_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (err < 0) {
        err_die_server(output, "Binding failed!");
    }

    err = listen(listen_socket, MAX_CLIENTS_ON_SERVER);
    if (err < 0) {
        err_die_server(output, "Listening failed!");
    }

    // === Server/Game has started === //

    fprintf(
        output, "%s%sServer is on...%s\n", 
        ANSI_GREEN, ANSI_BLINK, ANSI_RESET_ALL
    );

    // Initialize all tracks.
    init_tracks(&track);

    // Initialize client and define various client fields.
    while (1) {
        client_addr_len = sizeof(client_addr);
        received_conn = accept(
            listen_socket, (struct sockaddr*)&client_addr, &client_addr_len
        );

        // NOTE:? Will the server really accept more conn than specified on listen().
        if ((client_count + 1) >= MAX_CLIENTS_ON_SERVER) {
            fprintf(output, "Denied connection to %s!", ip_addr(client_addr));
            close(received_conn);
            continue;
        }

        init_client(&client, received_conn);
        client->address = client_addr;
        strcpy(client->ip, ip_addr(client_addr));

        pthread_create(&tid, NULL, &handle_client, (void*)client);

        sleep(1);
    }

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

void init_client(client_t **client, int client_sock) {
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

    // Client action struct allocation.
    (*client)->action = malloc(sizeof(struct Action));
    if ((*client)->action == NULL) {
        err_die_server(output, "Could not allocate memory for client's action!");
    }

    // "Dummy " game type allocation for client.
    // ATTENTION: Needs to be freed before assigning a different game pointer.
    (*client)->game = malloc(sizeof(game_t));
    if ((*client)->game == NULL) {
        err_die_server(output, "Could not allocate memory for game!");
    }


    // Initialising the char arrays.
    memset((*client)->ip, '\0', IP_LEN);
    memset((*client)->password, '\0', CLIENT_PASS_LEN);
    memset((*client)->player->name, '\0', CLIENT_NAME_LEN);

    // Allocated field definitions.
    (*client)->game->ID = 0;

    (*client)->player->ID           = ++client_id;
    (*client)->player->angle        = 0.0f;
    (*client)->player->speed        = 0.0f;
    (*client)->player->acceleration = 0.0f;
    (*client)->player->laps         = 0;
    (*client)->player->position.x   = 0;
    (*client)->player->position.y   = 0;
    (*client)->action->x            = 0;
    (*client)->action->y            = 0;

    (*client)->sock_fd = client_sock;

    // Increase the count of players on the server.
    pthread_mutex_lock(&clients_mutex);

    client_count++;

    // Save this client in the global clients list.
    push_client(&clients_start, client);

    pthread_mutex_unlock(&clients_mutex);
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
    (*game)->game_h->WinnerPlayerID    = -1;

    // Assigning a field to the game.
    chosen_track = get_track_by_id(&tracks_start, chosen_field_id);
    if (chosen_track == NULL) {
        err_die_server(output, "Could not find the requested field!");
    }

    (*game)->track = chosen_track;

    // Increase the count of games on the server.
    pthread_mutex_lock(&clients_mutex);

    game_count++;

    // Save this game in the global games list.
    push_game(&games_start, game);

    pthread_mutex_unlock(&clients_mutex);
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

    (*track)->start_line->beggining.x  = 15.1f;
    (*track)->start_line->beggining.y  = 2.1f;
    (*track)->start_line->end.x        = 15.1f;
    (*track)->start_line->end.y        = 5.1f;

    (*track)->main_line->beggining.x   = 10.1f;
    (*track)->main_line->beggining.y   = 5.1f;
    (*track)->main_line->end.x         = 25.1f;
    (*track)->main_line->end.y         = 6.1f;

    // Increase the count of tracks on the server.
    track_count++;

    // Save this track in the global tracks list.
    push_track(&tracks_start, track);
}