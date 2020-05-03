#include "../include/common.h"
#include "../include/protocol.h"
#include "../include/log_messages.h"
#include "../include/linked_list.h"

/* Freeing the inner allocated memory fiels */

void free_inner_client_fields(client_node_t **head) {
    free((*head)->client->player);
    (*head)->client->player = NULL;

    free((*head)->client->password);
    (*head)->client->password = NULL;

    free((*head)->client->ip);
    (*head)->client->ip = NULL;

    (*head)->client->game = NULL;
}

void free_inner_game_fields(game_node_t **head) {
    free((*head)->game->game_h);
    (*head)->game->game_h = NULL;

    (*head)->game->track = NULL;
}

void free_inner_track_fields(track_node_t **head) {
    free((*head)->track->field);
    (*head)->track->field = NULL;

    free((*head)->track->start_line);
    (*head)->track->start_line = NULL;

    free((*head)->track->main_line);
    (*head)->track->main_line = NULL;
}

/* Pushing an instance to its LL */

int push_client(client_node_t **head, client_t **value) {
    client_node_t *current;
    
    if (*head == NULL) {
        *head = malloc(sizeof(client_node_t));
        if (*head == NULL) {
            return MFAIL;
        }

        // memset(*head, 0, sizeof(client_node_t));

        (*head)->client = *value;
        (*head)->next_client = NULL;

        return RGOOD;
    }

    current = *head;

    while (current->next_client != NULL) {
        current = current->next_client;
    }

    current->next_client = malloc(sizeof(client_node_t));
    if (current->next_client == NULL) {
        return MFAIL;
    }

    // memset(current->next_client, 0, sizeof(client_node_t));

    current->client = *value;
    current->next_client->next_client = NULL;

    return RGOOD;
}

int push_game(game_node_t **head, game_t **value) {
    game_node_t *current;
    
    if (*head == NULL) {
        *head = malloc(sizeof(game_node_t));
        if (*head == NULL) {
            return MFAIL;
        }

        // memset(*head, 0, sizeof(game_node_t));

        (*head)->game = *value;
        (*head)->next_game = NULL;

        return RGOOD;
    }

    current = *head;

    while (current->next_game != NULL) {
        current = current->next_game;
    }

    current->next_game = malloc(sizeof(game_node_t));
    if (current->next_game == NULL) {
        return MFAIL;
    }
    // memset(current->next_game, 0, sizeof(game_node_t));

    current->game = *value;
    current->next_game->next_game = NULL;

    return RGOOD;
}

int push_track(track_node_t **head, track_t **value) {
    track_node_t *current;
    
    if (*head == NULL) {
        *head = (track_node_t*)malloc(sizeof(track_node_t));
        if (*head == NULL) {
            return MFAIL;
        }
        // memset(*head, 0, sizeof(track_node_t));

        (*head)->track = *value;
        (*head)->next_track = NULL;

        return RGOOD;
    }

    current = *head;

    while (current->next_track != NULL) {
        current = current->next_track;
    }

    current->next_track = (track_node_t*)malloc(sizeof(track_node_t));
    if (current->next_track == NULL) {
        return MFAIL;
    }
    // memset(current->next_track, 0, sizeof(track_node_t));
 
    current->track = *value;
    current->next_track->next_track = NULL;

    return RGOOD;
}

/* Removing an instance from the start of its LL */

int pop_client(FILE* fp, client_node_t **head) {
    client_node_t *next_node;

    if (*head == NULL) {
        return HNULL;
    }

    log_remove_client(fp, (*head)->client->player->name);
    next_node = (*head)->next_client;

    free_inner_client_fields(head);
    free(*head);
    *head = next_node;

    return RGOOD;
}

int pop_game(FILE *fp, game_node_t **head) {
    game_node_t *next_node;

    if (*head == NULL) {
        return HNULL;
    }

    log_remove_game(fp, (*head)->game->game_h->name);
    next_node = (*head)->next_game;

    free_inner_game_fields(head);
    free(*head);
    *head = next_node;

    return RGOOD;
}

int pop_track(FILE *fp, track_node_t **head) {
    track_node_t *next_node;

    if (*head == NULL) {
        return HNULL;
    }

    log_remove_track(fp, (*head)->track->field->name);
    next_node = (*head)->next_track;

    free_inner_track_fields(head);
    free(*head);
    *head = next_node;

    return RGOOD;
}

/* Removing an instance from the end of its LL */

int remove_last_client(FILE *fp, client_node_t **head) {
    client_node_t *current;

    if (*head == NULL) {
        return HNULL;
    }

    if ((*head)->next_client == NULL) {
        log_remove_client(fp, (*head)->client->player->name);
        free_inner_client_fields(head);
        free(*head);
        *head = NULL;

        return RGOOD;
    }

    current = *head;

    while (current->next_client->next_client != NULL) {
        current = current->next_client;
    }

    log_remove_client(fp, current->next_client->client->player->name);
    free_inner_client_fields(&(current->next_client));
    free(current->next_client);
    current->next_client = NULL;

    return RGOOD;
}

int remove_last_game(FILE *fp, game_node_t **head) {
    game_node_t *current;

    if (*head == NULL) {
        return HNULL;
    }

    if ((*head)->next_game == NULL) {
        log_remove_game(fp, (*head)->game->game_h->name);
        free_inner_game_fields(head);
        free(*head);
        *head = NULL;

        return RGOOD;
    }

    current = *head;

    while (current->next_game->next_game != NULL) {
        current = current->next_game;
    }

    log_remove_game(fp, current->next_game->game->game_h->name);
    free_inner_game_fields(&(current->next_game));
    free(current->next_game);
    current->next_game = NULL;

    return RGOOD;
}

int remove_last_track(FILE *fp, track_node_t **head) {
    track_node_t *current;

    if (*head == NULL) {
        return HNULL;
    }

    if ((*head)->next_track == NULL) {
        log_remove_track(fp, (*head)->track->field->name);
        free_inner_track_fields(head);
        free(*head);
        *head = NULL;

        return RGOOD;
    }

    current = *head;

    while (current->next_track->next_track != NULL) {
        current = current->next_track;
    }

    log_remove_track(fp, current->next_track->track->field->name);
    free_inner_track_fields(&(current->next_track));
    free(current->next_track);
    current->next_track = NULL;

    return RGOOD;
}

/* Removing an instance by id from its LL */

int remove_by_client_id(FILE *fp, client_node_t **head, int del_id) {
    client_node_t *temp_node;
    client_node_t *before_current;
    client_node_t *current;

    if (*head == NULL) {
        return HNULL;
    }

    // If the head_pointer has the ID.
    if ((*head)->client->player->ID == del_id) {
        temp_node = *head;
        log_remove_client(fp, (*head)->client->player->name);
        *head = (*head)->next_client;

        free_inner_client_fields(&temp_node);
        free(temp_node);
        return RGOOD;
    }

    // If other than head_pointer has the ID.
    before_current = *head;
    current = (*head)->next_client;

    while (1) {
        if (current == NULL) {
            break;
        }

        if (current->client->player->ID == del_id) {
            temp_node = current;
            log_remove_client(fp, current->client->player->name);
            before_current->next_client = current->next_client;

            free_inner_client_fields(&temp_node);
            free(temp_node);
            return RGOOD;
        }

        before_current = current;
        current = current->next_client;
    }

    return NFND;
}

int remove_by_game_id(FILE *fp, game_node_t **head, int del_id) {
    game_node_t *temp_node;
    game_node_t *before_current;
    game_node_t *current;

    if (*head == NULL) {
        return HNULL;
    }

    // If the head_pointer has the ID.
    if ((*head)->game->ID == del_id) {
        temp_node = *head;
        log_remove_game(fp, (*head)->game->game_h->name);
        *head = (*head)->next_game;

        free_inner_game_fields(&temp_node);
        free(temp_node);
        return RGOOD;
    }

    // If other than head_pointer has the ID.
    before_current = *head;
    current = (*head)->next_game;

    while (1) {
        if (current == NULL) {
            break;
        }

        if (current->game->ID == del_id) {
            temp_node = current;
            log_remove_game(fp, current->game->game_h->name);
            before_current->next_game = current->next_game;

            free_inner_game_fields(&temp_node);
            free(temp_node);
            return RGOOD;
        }

        before_current = current;
        current = current->next_game;
    }

    return NFND;
}

int remove_by_track_id(FILE *fp, track_node_t **head, int del_id) {
    track_node_t *temp_node;
    track_node_t *before_current;
    track_node_t *current;

    if (*head == NULL) {
        return HNULL;
    }

    // If the head_pointer has the ID.
    if ((*head)->track->field->ID == del_id) {
        temp_node = *head;
        log_remove_track(fp, (*head)->track->field->name);
        *head = (*head)->next_track;

        free_inner_track_fields(head);
        free(temp_node);
        return RGOOD;
    }

    // If other than head_pointer has the ID.
    before_current = *head;
    current = (*head)->next_track;

    while (1) {
        if (current == NULL) {
            break;
        }

        if (current->track->field->ID == del_id) {
            temp_node = current;
            log_remove_track(fp, current->track->field->name);
            before_current->next_track = current->next_track;

            free_inner_track_fields(&temp_node);
            free(temp_node);
            return RGOOD;
        }

        before_current = current;
        current = current->next_track;
    }

    return NFND;
}

/* Getting an instance by a given ID */

client_t *get_client_by_id(client_node_t **head, int want_id) {
    client_node_t *current = *head;

    while (current != NULL) {
        if (current->client->player->ID == want_id) {
            return current->client;
        }

        current = current->next_client;
    }

    return NULL;
}

game_t *get_game_by_id(game_node_t **head, int want_id) {
    game_node_t *current = *head;

    while (current != NULL) {
        if (current->game->ID == want_id) {
            return current->game;
        }

        current = current->next_game;
    }

    return NULL;
}

track_t *get_track_by_id(track_node_t **head, int want_id) {
    track_node_t *current = *head;

    while (current != NULL) {
        if (current->track->field->ID == want_id) {
            return current->track;
        }

        current = current->next_track;
    }

    return NULL;
}


/* Total removal of all the instances */

void remove_all_clients(FILE* fp, client_node_t **head) {
    int ret;

    do {
        ret = pop_client(fp, head);
    } while(ret != HNULL);
}

void remove_all_games(FILE* fp, game_node_t **head) {
    int ret;

    do {
        ret = pop_game(fp, head);
    } while(ret != HNULL);
}

void remove_all_tracks(FILE* fp, track_node_t **head) {
    int ret;

    do {
        ret = pop_track(fp, head);
    } while(ret != HNULL);
}