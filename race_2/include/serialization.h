#ifndef _SERIALIZATION_H_
#define _SERIALIZATION_H_

#include "common.h"
#include "protocol.h"

/* === COMMON TYPE SERIALIZATION/DESERIALIZATION === */

char *serialize_int(char *buffer, int *value);
char* deserialize_int(char *buffer, int *value);

char *serialize_float(char *buffer, float value);
char *deserialize_float(char *buffer, float *value);

char *serialize_string(char *buffer, char *value, int len);
char *deserialize_string(char *buffer, char *value, int len);

/* === PROTOCOL TYPE SERIALIZATION/DESERIALIZATION === */

void serialize_msg_CG(char *buffer, char *msg_type, char *client_name, char *game_name, int field_id);
void deserialize_msg_CG(char *buffer, char* client_name, char *game_name, int *field_id);
void serialize_msg_CG_response(char *buffer, client_t *client);
void deserialize_msg_CG_response(char *buffer, char *msg_type, client_t *client);

void serialize_msg_NF(char *buffer, char *msg_type);
void deserialize_msg_NF();
void serialize_msg_NF_response(char *buffer, int count_of_fields);
void deserialize_msg_NF_response(char *buffer, char *msg_type, int *n_field_ids);

void serialize_msg_FI(char *buffer, char *msg_type, int chose);
void deserialize_msg_FI(char *buffer, int *chose);
void serialize_msg_FI_response(char *buffer, track_t *track);
void deserialize_msg_FI_response(char *buffer, char *msg_type, track_t *track);

void serialize_msg_LI(char *buffer, char *msg_type);
void deserialize_msg_LI();
void serialize_msg_LI_response(char *buffer, int count_of_games, int **gid_arr);
void deserialize_msg_LI_response(
    char *buffer, char *msg_type, int *n_game_ids, 
    int **game_ids_arr
);

void serialize_msg_GI(char *buffer, char *msg_type, int chose);
void deserialize_msg_GI(char *buffer, int *chose);
void serialize_msg_GI_response(
    char *buffer, game_t *game, int g_client_count, 
    client_t *g_clients[MAX_CLIENTS_PER_GAME]
);
void deserialize_msg_GI_response(
    char *buffer, char *msg_type, game_t *game, int *g_client_count,
    struct Player_info ***other_pi_arr_of_p
);

void serialize_msg_JG(char *buffer, char *msg_type, int game_id, char *client_name);
void deserialize_msg_JG(char *buffer, int *game_id, char *client_name);
void serialize_msg_JG_response(char *buffer, client_t *client);
void deserialize_msg_JG_response(char *buffer, char *msg_type, client_t *client);

void serialize_msg_NOTIFY(char *buffer, char *msg_type, int new_p_id, char *name_buf);
void deserialize_msg_NOTIFY(char *buffer, char *msg_type, int *new_p_id, char *name_buf);

#endif