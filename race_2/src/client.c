#include "../include/common.h"
#include "../include/protocol.h"
#include "../include/serialization.h"
#include "../include/log_messages.h"
#include "../include/linked_list.h"

// NOTE: Have %s p/h to which I pass fmt([string]) func which 
// appends/prepends my format rules.
// NOTE: Handle overflows for number buffer like in create_game() func.
// NOTE: Client code takes the first chose when asking for info/description
// for field and game. Allow a client to ask multiple times in a loop
// until client declares that he has made his choice, by exiting the loop.

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

// Variables w/ whom to manage starting a game pressing CTRL+C.
int control_c_safe = 0;
int start_game_sig = 0;

// // Stores info about other players of a game.
// // Assign address of other_pi_arr_of_p for easier pointer management/iteration.
// struct Player_info ***SG_pi_others;

// Helper function prototypes.
void handle_sigint(int sig);
void send_n_recv(char* buffer, char *msg_type, int serv_sock);
void err_die_client(FILE *fp, char* err_msg);
void change_log_output(FILE *fp, char *path);

// Global structure initialization prototypes.
void init_client(client_t **client);
void init_game(game_t **game);
void init_tracks();

/* The main functions for communication */

// start_game handles actual game process or gameplay.
void start_game(char *buffer, client_t *client, struct Player_info ***SG_pi_others) {
    printf("\nGame started!\n");
    
    char                msg_type[MSG_TYPE_LEN];
    int                 udpsock, n, ret, tcp_sock;
    socklen_t           len;
    struct sockaddr_in  serv_addr;

    // Save this fd to later close it.
    tcp_sock = client->sock_fd;

    udpsock = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpsock < 0) {
        err_die_client(output, "Could not create UDP connection!");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family        = AF_INET;
    serv_addr.sin_addr.s_addr   = inet_addr(ip);
    serv_addr.sin_port          = htons(port+1);

    ret = connect(udpsock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret < 0) {
        err_die_client(output, "Could not connect UDP socket!");
    }

    /* UPDATE PLAYER */

    strcpy(msg_type, "UP\0");

    // NOTE: make send/recv non-blocking?
    do {
        // SOME ACTION OF THIS PLAYER...

        memset(buffer, '\0', MAX_BUFFER_SIZE);
        serialize_msg_UP(buffer, msg_type, client);

        sendto(
            udpsock, (const char*)buffer, MAX_BUFFER_SIZE, MSG_CONFIRM,
            (const struct sockaddr*)&serv_addr, sizeof(serv_addr)
        );

        memset(buffer, '\0', MAX_BUFFER_SIZE);

        n = recvfrom(
            udpsock, (char*)buffer, MAX_BUFFER_SIZE, MSG_WAITALL,
            (struct sockaddr*)&serv_addr, &len
        );
        if (n < 0) {
            err_die_client(output, "Receiving data from UDP sokcet failed!");
        }

        // NOTE:? Might need to manage in a seperate thread if this is blocking.
        // while (strlen(buffer) == 0) {
        ret = deserialize_msg_UP_response(
            buffer, msg_type, client, SG_pi_others
        );
        if (ret == -1) {
            continue;
        }

            // memset(buffer, '\0', MAX_BUFFER_SIZE);

            // n = recvfrom(
            //     udpsock, (char*)buffer, MAX_BUFFER_SIZE, MSG_WAITALL,
            //     (struct sockaddr*)&serv_addr, &len
            // );

            // Log that message about a player has been received.
        log_msg_UP_received(output, msg_type, (*SG_pi_others)[ret]);
        // DISPLAY the new data about the received player... after which - exit loop.
        // ....
        
        // }
    } while (1); // Or other gameplay info is met. END GAME

    close(udpsock);
    close(tcp_sock);
}

// join_game requests to join the game of game_id and waits until gets message
// of START GAME w/ necessary game info.
void join_game(char *buffer, client_t *client, int game_id) {
    char    msg_type[MSG_TYPE_LEN];
    char    *name_buf;
    int     p_name_allowed_size = CLIENT_NAME_LEN - 1;

    /* JOIN GAME */

    strcpy(msg_type, "JG\0");

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

    /* Serialize the data */

    memset(buffer, '\0', MAX_BUFFER_SIZE);
    serialize_msg_JG(buffer, msg_type, game_id, client->player->name);

    send_n_recv(buffer, msg_type, client->sock_fd);

    deserialize_msg_JG_response(buffer, msg_type, client);

    // Log the response.
    log_received_JG_msg(output, msg_type, client);

    /* Get notified about new players */

    // NOTE: Sit in a while loop and get notified about new players joining (max 4) 
    // until client wants to send a START GAME request.
    int     pid;

    // NOTE: Should store these values globally.
    int client_count;
    struct Player_info **other_pi_arr_of_p; // To assign values inside 
    // deseriaization func.
    // Stores info about other players of a game.
    // Assign address of other_pi_arr_of_p for easier pointer management/iteration.
    struct Player_info ***SG_pi_others;

    while (1) {
        memset(buffer,   '\0', MAX_BUFFER_SIZE);
        memset(msg_type, '\0', MSG_TYPE_LEN);
        memset(name_buf, '\0', MSG_BUF_LEN);

        strcpy(msg_type, "PI\0"); // NOTE: Sending PING PONG msg type resolves
        // recv/send functions blocking the program, for now.
        strcat(buffer, msg_type);

        send_n_recv(buffer, msg_type, client->sock_fd);

        if (buffer[0] == 'S' && buffer[1] == 'G') {
            deserialize_msg_SG_response(
                buffer, msg_type, &client_count, client->game,
                &other_pi_arr_of_p
            );

            // Assign to more readable pointer.
            SG_pi_others = &other_pi_arr_of_p;

            // (*SG_pi_others)[client_count] = client->player;

            // Log the response.
            log_received_SG_msg(
                output, msg_type, client->game, client_count, SG_pi_others
            );

            break;
        }

        deserialize_msg_NOTIFY(buffer, msg_type, &pid, name_buf);

        if (strlen(name_buf) != 0) {
            log_msg_NOTIFY_received(output, pid, name_buf);
        }

        sleep(1);
    }

    free(name_buf);
}

// list_games return the chosen game's ID which client will join.
// Function returns 0 if there is no game on the server.
int list_games(char *buffer, client_t *client) {
    char msg_type[MSG_TYPE_LEN];
    char num_str[DIGITS_LEN];
    int  *gid_arr; // Store array of received game IDs.
    int  n_games, chose, g_client_count;
    struct Player_info **other_pi_arr_of_p; // To assign values inside 
    // deseriaization func.
    struct Player_info ***p; // Assign address of other_pi_arr_of_p for
    // easier pointer management/iteration.

    /* LIST GAME */

    strcpy(msg_type, "LI\0");

    // Initialize the buffer for numbers.
    memset(num_str, '\0', DIGITS_LEN);

    memset(buffer, '\0', MAX_BUFFER_SIZE);
    serialize_msg_LI(buffer, msg_type);

    send_n_recv(buffer, msg_type, client->sock_fd);

    deserialize_msg_LI_response(buffer, msg_type, &n_games, &gid_arr);

    // Log the response.
    log_received_LI_msg(output, msg_type, n_games, gid_arr);

    // If the are no game on the server.
    if (n_games == 0) {
        return 0;
    }
    
    /* GAME INFO */

    strcpy(msg_type, "GI\0");

    // Reinitialize the buffer for numbers.
    memset(num_str, '\0', DIGITS_LEN);

    printf("Choose one of the game IDs to get more info: ");
    fgets(num_str, DIGITS_LEN, stdin);

    chose = atoi(num_str);
    if (chose <= 0) {
        err_die_client(output, "You provided an invalid number!");
    }

    // Check whether client chose an existing game ID.
    int valid_chose = 0;
    for (int i = 0; i < n_games; i++) {
        if (chose == gid_arr[i]) {
            valid_chose = 1;
            break;
        }
    }

    if (valid_chose == 0) {
        err_die_client(output, "There is no such game ID!");
    }

    client->game->ID = chose;

    // Reinitialize the buffer for data serialization.
    memset(buffer, '\0', MAX_BUFFER_SIZE);
    serialize_msg_GI(buffer, msg_type, chose);

    send_n_recv(buffer, msg_type, client->sock_fd);

    deserialize_msg_GI_response(
        buffer, msg_type, client->game, &g_client_count, &other_pi_arr_of_p
    );

    // Assign to more readable pointer.
    p = &other_pi_arr_of_p;

    // Log the response.
    log_received_GI_msg(output, msg_type, client, g_client_count, p);

    // Free the memory.
    for (int i = 0; i < g_client_count; i++) {
        free((*p)[i]);
    }

    free(*p);
    free(gid_arr);

    return chose;
}

// get_number_of_fields returns the chosen field ID from which to create a game.
int get_number_of_fields(char* buffer, client_t *client) {
    char msg_type[MSG_TYPE_LEN];
    char num_str[DIGITS_LEN];
    int  n_field_ids, chose;

    /* GET_NUMBER_OF_FIELDS */

    strcpy(msg_type, "NF\0");

    memset(num_str, '\0', DIGITS_LEN);

    memset(buffer, '\0', MAX_BUFFER_SIZE);
    serialize_msg_NF(buffer, msg_type);

    send_n_recv(buffer, msg_type, client->sock_fd);

    deserialize_msg_NF_response(buffer, msg_type, &(n_field_ids));

    /* FIELD INFO */

    strncpy(msg_type, "FI\0", 3);

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

// get_number_of_fields requests a new game creation and waits for other players
// to join.
void create_game(char *buffer, client_t *client, int field_id) {
    char    msg_type[MSG_TYPE_LEN];
    char    *name_buf;
    int     p_name_allowed_size = CLIENT_NAME_LEN - 1;
    int     g_name_allowed_size = GAME_NAME_LEN - 1;

    /* CREATE GAME */

    strcpy(msg_type, "CG\0");

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

    /* Serialize the data */

    memset(buffer, '\0', MAX_BUFFER_SIZE);
    serialize_msg_CG(buffer, msg_type, client->player->name, client->game->game_h->name, field_id);

    send_n_recv(buffer, msg_type, client->sock_fd);

    deserialize_msg_CG_response(buffer, msg_type, client);

    // Log the response.
    log_received_CG_msg(output, msg_type, client);

    /* Get notified about new players AND wait until START GAME */

    // NOTE: Sit in a while loop and get notified about new players joining (max 4) 
    // until want to proceess to send a START GAME request.
    int     pid;
    int     p_count = 1;

    // NOTE: Should store these values globally.
    int client_count;
    struct Player_info **other_pi_arr_of_p; // To assign values inside 
    // deseriaization func.
    // Stores info about other players of a game.
    // Assign address of other_pi_arr_of_p for easier pointer management/iteration.
    struct Player_info ***SG_pi_others;

    printf(
        "%s%sWaiting for other players to join...%s\n", 
        ANSI_LGREEN, ANSI_BLINK, ANSI_RESET_ALL
    );
    printf("Press CTRL+C to start the game\n");
    control_c_safe = 1;

    while (1) {
        memset(buffer,   '\0', MAX_BUFFER_SIZE);
        memset(msg_type, '\0', MSG_TYPE_LEN);
        memset(name_buf, '\0', MSG_BUF_LEN);

        strcpy(msg_type, "PI\0"); // NOTE: Sending PING PONG msg type resolves
        // recv/send functions to block the program, for now.
        strcat(buffer, msg_type);

        send_n_recv(buffer, msg_type, client->sock_fd);

        deserialize_msg_NOTIFY(buffer, msg_type, &pid, name_buf);

        // If a joined player name has been parsed.
        if (strlen(name_buf) != 0) {
            log_msg_NOTIFY_received(output, pid, name_buf);
            p_count++;
        }

        // In order to start the game.
        if ((p_count == 4 || start_game_sig == 1)) {
            memset(buffer, '\0', MAX_BUFFER_SIZE);
            strcpy(msg_type, "SG\0");
            serialize_msg_SG(buffer, msg_type, client);

            do {
                send_n_recv(buffer, msg_type, client->sock_fd);
                printf("loop\n");
                sleep(1);
            } while (!(buffer[0] == 'S' && buffer[1] == 'G'));

            deserialize_msg_SG_response(
                buffer, msg_type, &client_count, client->game,
                &other_pi_arr_of_p
            );

            // Assign to more readable pointer.
            SG_pi_others = &other_pi_arr_of_p;

            // Add the creator of the game to the players struct array of the game.
            // (*SG_pi_others)[client_count] = client->player;

            // Log the response.
            log_received_SG_msg(
                output, msg_type, client->game, client_count, SG_pi_others
            );

            break;
        }

        sleep(1);
    }

    free(name_buf);

    start_game(buffer, client, SG_pi_others);
}

int main(int argc, char **argv) {
    struct sockaddr_in  serv_addr;
    int                 err, flag, serv_sock;

    client_t            *client;
    int                 field_id;
    int                 game_id;

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

    printf("%sWelcome to the best race game EVER!%s\n", ANSI_GREEN, ANSI_RESET_ALL);

    // Allocate and initialize the buffer.
    buffer = malloc(MAX_BUFFER_SIZE);
    if (buffer == NULL) {
        err_die_client(output, "Could not allocate memory for buffer!");
    }

    memset(buffer, '\0', MAX_BUFFER_SIZE);
    
    // Define client and all its inner structures.
    init_client(&client);
    client->sock_fd = serv_sock;

    /* === The lifecycle of the race game === */

    // Prepare a buffer where to store the clients answer.
    char *answer_buf;
    answer_buf = malloc(MSG_BUF_LEN);
    if (answer_buf == NULL) {
        err_die_client(output, "Could not allocate memory for name buffer!");
    }

    memset(answer_buf, '\0', MSG_BUF_LEN);

    // Ask for whether client wants to JOIN or CREATE a game.
    fprintf(output, "Do you want to CREATE or JOIN a game?: ");
    fgets(answer_buf, MSG_BUF_LEN, stdin);

    // Set the last element of name_buf to 0 for checking the length. 
    answer_buf[MSG_BUF_LEN-1] = '\0';
    if (strlen(answer_buf) > sizeof("CREATE")+1) {
        err_die_client(output, "The answer is invalid!");
    }

    answer_buf[strlen(answer_buf)-1] = '\0';

    if (strcmp(answer_buf, "CREATE") == 0) {        // CREATE branch.
        field_id = get_number_of_fields(buffer, client);
        create_game(buffer, client, field_id);
        // create_game(buffer, client, field_id); // BUG: When creating two
        // games and then some other client sends LG msg - server crashes.
    } else if (strcmp(answer_buf, "JOIN") == 0) {   // JOIN branch.
        game_id = list_games(buffer, client);
        join_game(buffer, client, game_id);
    } else {
        err_die_client(output, "The answer is invalid!");
    }

    // start_game(buffer, client);

    /* Exit program gracefully */

    printf("Press ENTER to exit...");
    getchar();

    free(buffer);
    free(answer_buf);

    remove_all_clients(output, &clients_start);
    remove_all_tracks(output, &tracks_start);
    remove_all_games(output, &games_start);

    printf("Goodbye!\n");
    exit(EXIT_SUCCESS);
}

/* === Helper functions === */

void handle_sigint(int sig) {
    if (control_c_safe == 1) {
        start_game_sig = 1;
        control_c_safe = 0;

        return;
    }

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
    game_t  *game;
    track_t *track;

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

    // Client action struct allocation.
    (*client)->action = malloc(sizeof(struct Action));
    if ((*client)->action == NULL) {
        err_die_client(output, "Could not allocate memory for client's action!");
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
    (*client)->action->x            = 0;
    (*client)->action->y            = 0;

    // Save this client in the global clients list.
    push_client(&clients_start, client);

    // Allocate, initialize and assign the game to client.
    init_game(&game);
    (*client)->game = game;

    // Allocate, initialize and assign the track to client.
    init_tracks(&track);
    (*client)->game->track = track;
}

void init_game(game_t **game) {
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

    // Save this game in the global games list.
    push_game(&games_start, game);// Stores info about other players of a game.
// // Assign address of other_pi_arr_of_p for easier pointer management/iteration.
// struct Player_info ***SG_pi_others;// Stores info about other players of a game.
// // Assign address of other_pi_arr_of_p for easier pointer management/iteration.
// struct Player_info ***SG_pi_others;
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