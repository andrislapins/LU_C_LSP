#include "../include/common.h"
#include "../include/protocol.h"
#include "../include/linked_list.h"

/* Linked List for Clients */

void push_client(client_node_t *head, client_t *value) {
    client_node_t *current;
    
    if (head == NULL) {
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
        err_die("Could not allocate memory for next client!");
    }

    current->client = value;
    current->next_client->next_client = NULL;
}

char *pop_client(client_node_t *head) {
    char del_p_name[CLIENT_NAME_LEN] = {0};
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
    char del_p_name[CLIENT_NAME_LEN] = {0};
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
    char del_p_name[CLIENT_NAME_LEN] = {0};
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
