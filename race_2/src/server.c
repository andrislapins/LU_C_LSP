#include "../include/common.h"
#include "../include/protocol.h"
#include "../include/serialization.h"
#include "../include/my_strings.h"

// NOTE: Is there something better than _Atomic for race condition issues?
static _Atomic int client_count = 0;
static int player_id = 100;
static int game_id = 200;

int count_of_fields = 1;

void handle_sigint(int sig) {
    printf("Caught signal - %d\n", sig);
    exit(EXIT_FAILURE);
}

void create_game_response(char *buffer, char *msg_type, client_t *client) {
    deserialize_msg_CG(buffer+3, client);

    client->curr_game_id = game_id++;
    
    printf("Player - %s (id: %d) ", client->player_name, client->player_id);
    printf("created a game - %s ", client->curr_game_name);
    printf("(id: %d) ", client->curr_game_id);
    printf("on field - %d\n", client->chosen_field_id);

    // Create a token (pass) for the player.
    generate_password(client);

    // Set a response back to the client.
    bzero(buffer, MAX_BUFFER_SIZE);
    serialize_msg_CG_response(buffer, client);
}

void get_number_of_fields_response(char *buffer, char* msg_type, client_t *client) {
    // Set a response back to the client.
    bzero(buffer, MAX_BUFFER_SIZE);
    serialize_msg_NF_response(buffer, count_of_fields);
    printf("Sent the count of available fields to player %s\n", client->player_name);
}

void field_info_response(char *buffer, char* msg_type, client_t *client) {
    int chose;

    deserialize_msg_FI(buffer, &chose);
}

void handle_message(char *buffer, client_t *client) {
    char msg_type[MSG_TYPE_LEN] = { buffer[0], buffer[1], '\0' };
    printf("Received msg_type - %s from player %s\n", msg_type, client->player_name);

    if (strcmp(msg_type, "CG") == 0) {
        create_game_response(buffer, msg_type, client);
    } else if (strcmp(msg_type, "NF") == 0) {
        get_number_of_fields_response(buffer, msg_type, client);
    } else if (strcmp(msg_type, "FI") == 0) {
        field_info_response(buffer, msg_type, client);
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
                data_n = send(client->sock_fd, buffer, MAX_BUFFER_SIZE, 0);
                if (data_n < 0) {
                    printf("An error occurred sending a message!\n");
                    leave_flag = 1;
                }
            }
        } else if (data_n == 0) {
            printf("Lost connection with %s\n", client->player_name);
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
    int listen_socket, client_socket;
    int err;
    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;

    char *ip = "127.0.0.1";
    int option = 1;

    // Handle Ctrl+c.
    signal(SIGINT, handle_sigint);

    // Define the socket settings for listening.
    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family        = AF_INET;
    serv_addr.sin_addr.s_addr   = inet_addr(ip);
    serv_addr.sin_port          = htons(PORT);

    err = setsockopt(listen_socket, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char*)&option, sizeof(option));
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

    printf("Server is on...\n");

    // [functions which initialize such global vars like an array of Field + Line pointers]

    socklen_t client_len = sizeof(client_addr);
    client_socket = accept(listen_socket, (struct sockaddr*)&client_addr, &client_len);
    
    client_t *client = (client_t*)malloc(sizeof(client_t));
    client->address     = client_addr;
    client->sock_fd     = client_socket;
    client->player_id   = player_id++;

    handle_client(client);

    printf("Goodbye!\n");
    close(listen_socket);
    exit(EXIT_SUCCESS);
}