#include "../include/common.h"
#include "../include/protocol.h"
#include "../include/linked_list.h"

// NOTE:! The returned strings were dynamically allocated.
// Free the memory after being done manipulating the string!

/* Linked List for Clients */

void push_client(client_node_t *head, client_t *value) {
    client_node_t *current;
    
    if (head == NULL) {
        head = (client_node_t*)calloc(1, sizeof(client_node_t));
        head->client = value;
        head->next_client = NULL;
        return;
    }

    current = head;

    while (current->next_client != NULL) {
        current = current->next_client;
    }

    current->next_client = (client_node_t*)calloc(1, sizeof(client_node_t));
    if (current->next_client == NULL) {
        err_die(stdout, "Could not allocate memory for next client!");
    }

    current->client = value;
    current->next_client->next_client = NULL;
}

char *pop_client(client_node_t *head) {
    char *del_p_name = (char*)calloc(1, CLIENT_NAME_LEN);
    client_node_t *next_node;

    if (head == NULL) {
        return NULL;
    }

    next_node = head->next_client;
    strcpy(del_p_name, head->client->player->name);
    free(head);
    head = next_node;

    return del_p_name;
}

char *remove_last_client(client_node_t *head) {
    char *del_p_name = (char*)calloc(1, CLIENT_NAME_LEN);
    client_node_t *current;

    if (head->next_client == NULL) {
        strcpy(del_p_name, head->client->player->name);
        free(head);
        return del_p_name;
    }

    current = head;

    while (current->next_client->next_client != NULL) {
        current = current->next_client;
    }

    strcpy(del_p_name, current->next_client->client->player->name);

    free(current->next_client);
    current->next_client = NULL;

    return del_p_name;
}

char *remove_by_client_id(client_node_t *head, int del_id) {
    char *del_p_name = (char*)calloc(1, CLIENT_NAME_LEN);
    client_node_t *current;
    client_node_t *before_current;
    client_node_t *temp_node;

    if (head == NULL) {
        return NULL;
    }

    before_current = head;
    current = head->next_client;

    while (1) {
        if (current == NULL) {
            break;
        }

        if (current->client->player->ID == del_id) {
            temp_node = current;
            strcpy(del_p_name, current->client->player->name);
            before_current->next_client = current->next_client;

            free(temp_node);
            return del_p_name;
        }

        before_current = current;
        current = current->next_client;
    }

    return NULL;
}

client_t *get_client_by_id(client_node_t *head, int want_id) {
    client_node_t *current = head;

    while (current != NULL) {
        if (current->client->player->ID == want_id) {
            return current->client;
        }

        current = current->next_client;
    }

    return NULL;
}

/* Linked List for Tracks */

void push_track(track_node_t *head, track_t *value) {
    track_node_t *current;
    
    if (head == NULL) {
        head = (track_node_t*)calloc(1, sizeof(track_node_t));
        head->track = value;
        head->next_track = NULL;
        return;
    }

    current = head;

    while (current->next_track != NULL) {
        current = current->next_track;
    }

    current->next_track = (track_node_t*)calloc(1, sizeof(track_node_t));
    if (current->next_track == NULL) {
        err_die(stdout, "Could not allocate memory for next track!");
    }

    current->track = value;
    current->next_track->next_track = NULL;
}

char *pop_track(track_node_t *head) {
    char *del_t_f_name = (char*)calloc(1, CLIENT_NAME_LEN);
    track_node_t *next_node;

    if (head == NULL) {
        return NULL;
    }

    next_node = head->next_track;
    strcpy(del_t_f_name, head->track->field->name);
    free(head);
    head = next_node;

    return del_t_f_name;
}

char *remove_last_track(track_node_t *head) {
    char *del_t_f_name = (char*)calloc(1, CLIENT_NAME_LEN);
    track_node_t *current;

    if (head->next_track == NULL) {
        strcpy(del_t_f_name, head->track->field->name);
        free(head);
        return del_t_f_name;
    }

    current = head;

    while (current->next_track->next_track != NULL) {
        current = current->next_track;
    }

    strcpy(del_t_f_name, current->next_track->track->field->name);

    free(current->next_track);
    current->next_track = NULL;

    return del_t_f_name;
}

char *remove_by_track_id(track_node_t *head, int del_id) {
    char *del_t_f_name = (char*)calloc(1, CLIENT_NAME_LEN);
    track_node_t *current;
    track_node_t *before_current;
    track_node_t *temp_node;

    if (head == NULL) {
        return NULL;
    }

    before_current = head;
    current = head->next_track;

    while (1) {
        if (current == NULL) {
            break;
        }

        if (current->track->field->ID == del_id) {
            temp_node = current;
            strcpy(del_t_f_name, current->track->field->name);
            before_current->next_track = current->next_track;

            free(temp_node);
            return del_t_f_name;
        }

        before_current = current;
        current = current->next_track;
    }

    return NULL;
}

track_t *get_track_by_id(track_node_t *head, int want_id) {
    track_node_t *current = head;

    while (current != NULL) {
        if (current->track->field->ID == want_id) {
            return current->track;
        }

        current = current->next_track;
    }

    return NULL;
}

/* Linked List for Games */

void push_game(game_node_t *head, game_t *value) {
    game_node_t *current;
    
    if (head == NULL) {
        head = (game_node_t*)calloc(1, sizeof(game_node_t));
        head->game = value;
        head->next_game = NULL;
        return;
    }

    current = head;

    while (current->next_game != NULL) {
        current = current->next_game;
    }

    current->next_game = (game_node_t*)calloc(1, sizeof(game_node_t));
    if (current->next_game == NULL) {
        err_die(stdout, "Could not allocate memory for next game!");
    }

    current->game = value;
    current->next_game->next_game = NULL;
}

char *pop_game(game_node_t *head) {
    char *del_g_name = (char*)calloc(1, CLIENT_NAME_LEN);
    game_node_t *next_node;

    if (head == NULL) {
        return NULL;
    }

    next_node = head->next_game;
    strcpy(del_g_name, head->game->game_h->name);
    free(head);
    head = next_node;

    return del_g_name;
}

char *remove_last_game(game_node_t *head) {
    char *del_g_name = (char*)calloc(1, CLIENT_NAME_LEN);
    game_node_t *current;

    if (head->next_game == NULL) {
        strcpy(del_g_name, head->game->game_h->name);
        free(head);
        return del_g_name;
    }

    current = head;

    while (current->next_game->next_game != NULL) {
        current = current->next_game;
    }

    strcpy(del_g_name, current->next_game->game->game_h->name);

    free(current->next_game);
    current->next_game = NULL;

    return del_g_name;
}

char *remove_by_game_id(game_node_t *head, int del_id) {
    char *del_g_name = (char*)calloc(1, CLIENT_NAME_LEN);
    game_node_t *current;
    game_node_t *before_current;
    game_node_t *temp_node;

    if (head == NULL) {
        return NULL;
    }

    before_current = head;
    current = head->next_game;

    while (1) {
        if (current == NULL) {
            break;
        }

        if (current->game->ID == del_id) {
            temp_node = current;
            strcpy(del_g_name, current->game->game_h->name);
            before_current->next_game = current->next_game;

            free(temp_node);
            return del_g_name;
        }

        before_current = current;
        current = current->next_game;
    }

    return NULL;
}

game_t *get_game_by_id(game_node_t *head, int want_id) {
    game_node_t *current = head;

    while (current != NULL) {
        if (current->game->ID == want_id) {
            return current->game;
        }

        current = current->next_game;
    }

    return NULL;
}