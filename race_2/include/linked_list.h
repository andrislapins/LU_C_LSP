#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#include "common.h"

// PLAN:?
// 1. Copy/Write a good implementation from net. WRITE MY OWN. TRY.
// 2. Have a node for every protocol which to hold globally
// EXAMPLE: struct client_t_node { client_t client; client_t *next; };

// MEANWHILE - implement just the communication by sending dummy values
// THEN - implement data structure that could be applied better.

// Protocol data type nodes with the use of linked lists.
typedef struct client_node {
    client_t *client;
    struct client_node *next_client;
} client_node_t;

typedef struct track_node {
    track_t *track;
    struct track_node *next_track;
} track_node_t;

typedef struct game_node {
    game_t *game;
    struct game_node *next_game;
} game_node_t;

/* Linked List functions for Clients */

void push_client(client_node_t **head, client_t **value);
char *pop_client(client_node_t *head);
char *remove_last_client(client_node_t *head);
char *remove_by_client_id(client_node_t *head, int del_id);
client_t *get_client_by_id(client_node_t *head, int want_id);

/* Linked List functions for Tracks */

void push_track(track_node_t **head, track_t **value);
char *pop_track(track_node_t *head);
char *remove_last_track(track_node_t *head);
char *remove_by_track_id(track_node_t *head, int del_id);
track_t *get_track_by_id(track_node_t *head, int want_id);

/* Linked List functions for Games */

void push_game(game_node_t **head, game_t **value);
char *pop_game(game_node_t *head);
char *remove_last_game(game_node_t *head);
char *remove_by_game_id(game_node_t *head, int del_id);
game_t *get_game_by_id(game_node_t *head, int want_id);

#endif