#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#include "common.h"

/* Protocol data type nodes with the use of linked lists. */

typedef struct client_node {
    client_t            *client;
    struct client_node  *next_client;
} client_node_t;

typedef struct track_node {
    track_t             *track;
    struct track_node   *next_track;
} track_node_t;

typedef struct game_node {
    game_t              *game;
    struct game_node    *next_game;
} game_node_t;

/* Typical LL function return values */

enum result {
    RGOOD   =  0, // Result Good.
    MFAIL   = -1, // Memory allocation Failed.
    HNULL   = -2, // Head is NULL.
    NFND    = -3  // Not Found.
};

/* Pushing instnace to its LL */

int push_client(client_node_t **head, client_t **value);
int push_game(game_node_t **head, game_t **value);
int push_track(track_node_t **head, track_t **value);

/* Removing instance from the start of its LL */

int pop_client(FILE *fp, client_node_t **head);
int pop_game(FILE *fp, game_node_t **head);
int pop_track(FILE *fp, track_node_t **head);

/* Removing instance from the end of its LL */

int remove_last_client(FILE *fp, client_node_t **head);
int remove_last_game(FILE *fp, game_node_t **head);
int remove_last_track(FILE *fp, track_node_t **head);

/* Removing instance by id from its LL */

int remove_by_client_id(FILE *fp, client_node_t **head, int del_id);
int remove_by_game_id(FILE *fp, game_node_t **head, int del_id);
int remove_by_track_id(FILE *fp, track_node_t **head, int del_id);

/* Getting instance by a given ID */

client_t *get_client_by_id(client_node_t **head, int want_id);
game_t *get_game_by_id(game_node_t **head, int want_id);
track_t *get_track_by_id(track_node_t **head, int want_id);

/* Total removal of all the instances */

void remove_all_clients(FILE* fp, client_node_t **head);
void remove_all_games(FILE* fp, game_node_t **head);
void remove_all_tracks(FILE* fp, track_node_t **head);

#endif