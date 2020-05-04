#include "../include/common.h"
#include "../include/protocol.h"
#include "../include/serialization.h"

/* === COMMON TYPE SERIALIZATION/DESERIALIZATION === */

/* int serialization/deserialization */

char *serialize_int(char *buffer, int *value) {
    char *value_s = (char*)value;
    int len = sizeof(int);

    for (int i = 0; i < len; i++) {
        buffer[i] = value_s[i];
    }

    return buffer + len;
}

char* deserialize_int(char *buffer, int *value) {
    int num, len = sizeof(int);

    memcpy(&num, buffer, len);
    *value = num;

    return buffer + len;
}
/* float serialization/deserialization */

char *serialize_float(char *buffer, float value) {
    int len = sizeof(float);

    memcpy(buffer, &value, len);

    return buffer + len;
}

char *deserialize_float(char *buffer, float *value) {
    int len = sizeof(float);
    float num;

    memcpy(&num, buffer, len);
    *value = num;

    return buffer + len;
}

/* string serialization/deserialization */

char *serialize_string(char *buffer, char *value, int len) {
    for (int i = 0; i < len; i++) {
        buffer[i] = value[i];
    }

    return buffer + len;
}

char *deserialize_string(char *buffer, char *value, int len) {;
    memcpy(value, buffer, len);

    return buffer + len;
}

/* === PROTOCOL TYPE SERIALIZATION/DESERIALIZATION === */

/* msg CREATE GAME serialization/deserialization */

void serialize_msg_CG(char *buffer, char *msg_type, char *client_name, char *game_name, int field_id) {
    buffer = serialize_string(buffer, msg_type , MSG_TYPE_LEN);
    buffer = serialize_string(buffer, client_name , CLIENT_NAME_LEN);
    buffer = serialize_string(buffer, game_name , GAME_NAME_LEN);
    buffer = serialize_int(buffer, &(field_id));
}

void deserialize_msg_CG(char *buffer, char* client_name, char *game_name, int *field_id) {
    buffer = deserialize_string(buffer, client_name, CLIENT_NAME_LEN);
    buffer = deserialize_string(buffer, game_name, GAME_NAME_LEN);
    buffer = deserialize_int(buffer, field_id);
}

void serialize_msg_CG_response(char *buffer, client_t *client) {
    buffer = serialize_int(buffer, &(client->game->ID));
    buffer = serialize_int(buffer, &(client->player->ID));
    buffer = serialize_string(buffer, client->password , CLIENT_PASS_LEN);
}

void deserialize_msg_CG_response(char *buffer, char *msg_type, client_t *client) {
    buffer = deserialize_string(buffer, msg_type , MSG_TYPE_LEN);
    buffer = deserialize_int(buffer, &(client->game->ID));
    buffer = deserialize_int(buffer, &(client->player->ID));
    buffer = deserialize_string(buffer, client->password , CLIENT_PASS_LEN);
}

/* msg NUMBER OF FIELDS serialization/deserialization */

void serialize_msg_NF(char *buffer, char *msg_type) {
    buffer = serialize_string(buffer, msg_type , MSG_TYPE_LEN);
}

void deserialize_msg_NF();

void serialize_msg_NF_response(char *buffer, int count_of_fields) {
    buffer = serialize_int(buffer, &(count_of_fields));
}

void deserialize_msg_NF_response(char *buffer, char *msg_type, int *n_field_ids) {
    buffer = deserialize_string(buffer, msg_type , MSG_TYPE_LEN);
    buffer = deserialize_int(buffer, n_field_ids);
}

/* msg FIELD INFO serialization/deserialization */

void serialize_msg_FI(char *buffer, char *msg_type, int chose) {
    buffer = serialize_string(buffer, msg_type , MSG_TYPE_LEN);
    buffer = serialize_int(buffer, &(chose));
}

void deserialize_msg_FI(char *buffer, int *chose) {
    buffer = deserialize_int(buffer, chose);
}

void serialize_msg_FI_response(char *buffer, track_t *track) {
    buffer = serialize_int(buffer, &(track->field->ID));
    buffer = serialize_string(buffer, track->field->name , FIELD_NAME_LEN);
    buffer = serialize_int(buffer, &(track->field->Width));
    buffer = serialize_int(buffer, &(track->field->Height));
    buffer = serialize_float(buffer, track->start_line->beggining.x);
    buffer = serialize_float(buffer, track->start_line->beggining.y);
    buffer = serialize_float(buffer, track->start_line->end.x);
    buffer = serialize_float(buffer, track->start_line->end.y);
    buffer = serialize_float(buffer, track->main_line->beggining.x);
    buffer = serialize_float(buffer, track->main_line->beggining.y);
    buffer = serialize_float(buffer, track->main_line->end.x);
    buffer = serialize_float(buffer, track->main_line->end.y);
    buffer = serialize_int(buffer, &(track->n_extra_lines));
}

void deserialize_msg_FI_response(char *buffer, char *msg_type, track_t *track) {
    buffer = deserialize_string(buffer, msg_type , MSG_TYPE_LEN);
    buffer = deserialize_int(buffer, &(track->field->ID));
    buffer = deserialize_string(buffer, track->field->name , FIELD_NAME_LEN);
    buffer = deserialize_int(buffer, &(track->field->Width));
    buffer = deserialize_int(buffer, &(track->field->Height));
    buffer = deserialize_float(buffer, &(track->start_line->beggining.x));
    buffer = deserialize_float(buffer, &(track->start_line->beggining.y));
    buffer = deserialize_float(buffer, &(track->start_line->end.x));
    buffer = deserialize_float(buffer, &(track->start_line->end.y));
    buffer = deserialize_float(buffer, &(track->main_line->beggining.x));
    buffer = deserialize_float(buffer, &(track->main_line->beggining.y));
    buffer = deserialize_float(buffer, &(track->main_line->end.x));
    buffer = deserialize_float(buffer, &(track->main_line->end.y));
    buffer = deserialize_int(buffer, &(track->n_extra_lines));
}

/* msg LIST GAMES serialization/deserialization */

void serialize_msg_LI(char *buffer, char *msg_type) {
    buffer = serialize_string(buffer, msg_type , MSG_TYPE_LEN);
}

void deserialize_msg_LI();

void serialize_msg_LI_response(char *buffer, int count_of_games, int **gid_arr) {
    buffer = serialize_int(buffer, &(count_of_games));
    // Serialize each game ID.
    for(int i = 0; i < count_of_games; i++) {
        buffer = serialize_int(buffer, gid_arr[i]);
    }
}

void deserialize_msg_LI_response(char *buffer, char *msg_type, int *n_game_ids, int **gid_arr) {
    buffer = deserialize_string(buffer, msg_type , MSG_TYPE_LEN);
    buffer = deserialize_int(buffer, n_game_ids);
    // Deserialize each received game ID.
    *gid_arr = malloc(*n_game_ids * sizeof(int));
    for(int i = 0; i < *n_game_ids; i++) {
        buffer = deserialize_int(buffer, gid_arr[i]);
    }
}

/* msg GAME INFO serialization/deserialization */

void serialize_msg_GI(char *buffer, char *msg_type, int chose) {
    buffer = serialize_string(buffer, msg_type , MSG_TYPE_LEN);
    buffer = serialize_int(buffer, &(chose));
}

void deserialize_msg_GI(char *buffer, int *chose) {
    buffer = deserialize_int(buffer, chose);
}

void serialize_msg_GI_response(
    char *buffer, game_t *game, int g_client_count, 
    client_t *g_clients[MAX_CLIENTS_PER_GAME]
) {
    buffer = serialize_int(buffer, &(game->game_h->status));
    buffer = serialize_string(buffer, game->game_h->name, GAME_NAME_LEN);
    buffer = serialize_int(buffer, &(game->game_h->WinnerPlayerID));
    buffer = serialize_int(buffer, &(g_client_count));
    // Serialize each player info struct.
    for(int i = 0; i < g_client_count; i++) {
        buffer = serialize_int(buffer, &(g_clients[i]->player->ID));
        buffer = serialize_string(buffer, g_clients[i]->player->name, CLIENT_NAME_LEN);
        buffer = serialize_float(buffer, g_clients[i]->player->position.x);
        buffer = serialize_float(buffer, g_clients[i]->player->position.y);
        buffer = serialize_float(buffer, g_clients[i]->player->angle);
        buffer = serialize_float(buffer, g_clients[i]->player->speed);
        buffer = serialize_float(buffer, g_clients[i]->player->acceleration);
        buffer = serialize_int(buffer, &(g_clients[i]->player->laps));
    }
    buffer = serialize_int(buffer, &(game->track->field->ID));
    buffer = serialize_string(buffer, game->track->field->name, FIELD_NAME_LEN);
    buffer = serialize_int(buffer, &(game->track->field->Width));
    buffer = serialize_int(buffer, &(game->track->field->Height));
}

void deserialize_msg_GI_response(
    char *buffer, char *msg_type, game_t *game,
    int *g_client_count, struct Player_info ***other_pi_arr_of_p
) {
    struct Player_info **pi_arr_of_p;

    buffer = deserialize_string(buffer, msg_type , MSG_TYPE_LEN);
    buffer = deserialize_int(buffer, &(game->game_h->status));
    buffer = deserialize_string(buffer, game->game_h->name, GAME_NAME_LEN);
    buffer = deserialize_int(buffer, &(game->game_h->WinnerPlayerID));
    buffer = deserialize_int(buffer, g_client_count);

    // Create an array of PI pointers.
    pi_arr_of_p = malloc(*g_client_count * sizeof(struct Player_info));

    // Allocate and deserialize each player info struct.
    for(int i = 0; i < *g_client_count; i++) {
        pi_arr_of_p[i] = malloc(sizeof(struct Player_info));

        buffer = deserialize_int(buffer, &(pi_arr_of_p[i]->ID));
        buffer = deserialize_string(buffer, pi_arr_of_p[i]->name, CLIENT_NAME_LEN);
        buffer = deserialize_float(buffer, &(pi_arr_of_p[i]->position.x));
        buffer = deserialize_float(buffer, &(pi_arr_of_p[i]->position.y));
        buffer = deserialize_float(buffer, &(pi_arr_of_p[i]->angle));
        buffer = deserialize_float(buffer, &(pi_arr_of_p[i]->speed));
        buffer = deserialize_float(buffer, &(pi_arr_of_p[i]->acceleration));
        buffer = deserialize_int(buffer, &(pi_arr_of_p[i]->laps));
    }

    buffer = deserialize_int(buffer, &(game->track->field->ID));
    buffer = deserialize_string(buffer, game->track->field->name, FIELD_NAME_LEN);
    buffer = deserialize_int(buffer, &(game->track->field->Width));
    buffer = deserialize_int(buffer, &(game->track->field->Height));

    // Assign the pointer back to the calling function.
    *other_pi_arr_of_p = pi_arr_of_p;
}