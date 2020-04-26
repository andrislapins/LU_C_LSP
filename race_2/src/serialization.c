#include "../include/common.h"
#include "../include/protocol.h"
#include "../include/serialization.h"

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

/* struct Field serialization/deserialization */
void serialize_field(char *buffer, struct Field *field) {
    buffer = serialize_int(buffer, &(field->ID));
    buffer = serialize_int(buffer, &(field->Height));
    buffer = serialize_int(buffer, &(field->Width));
    buffer = serialize_string(buffer, field->name, FIELD_NAME_LEN);
}

void deserialize_field(char *buffer, struct Field *field) {
    buffer = deserialize_int(buffer, &(field->ID));
    buffer = deserialize_int(buffer, &(field->Height));
    buffer = deserialize_int(buffer, &(field->Width));
    buffer = deserialize_string(buffer, field->name, FIELD_NAME_LEN);
}

/* msg CREATE GAME serialization/deserialization */
void serialize_msg_CG(char *buffer, char *msg_type, char *player_name, char *game_name, int field_id) {
    buffer = serialize_string(buffer, msg_type , MSG_TYPE_LEN);
    buffer = serialize_string(buffer, player_name , PLAYER_NAME_LEN);
    buffer = serialize_string(buffer, game_name , GAME_NAME_LEN);
    buffer = serialize_int(buffer, &(field_id));
}

void serialize_msg_CG_response(char *buffer, client_t *client) {
    buffer = serialize_int(buffer, &(client->curr_game_id));
    buffer = serialize_int(buffer, &(client->player_id));
    buffer = serialize_string(buffer, client->player_pass , PLAYER_PASS_LEN);
}

void deserialize_msg_CG(char *buffer, client_t *client) {
    buffer = deserialize_string(buffer, client->player_name, PLAYER_NAME_LEN);
    buffer = deserialize_string(buffer, client->curr_game_name, GAME_NAME_LEN);
    buffer = deserialize_int(buffer, &(client->chosen_field_id));
}

void deserialize_msg_CG_response(char *buffer, client_t *client) {
    buffer = deserialize_int(buffer, &(client->curr_game_id));
    buffer = deserialize_int(buffer, &(client->player_id));
    buffer = deserialize_string(buffer, client->player_pass , PLAYER_PASS_LEN);
}

/* msg NUMBER OF FIELDS serialization/deserialization */
void serialize_msg_NF(char *buffer, char *msg_type) {
    buffer = serialize_string(buffer, msg_type , MSG_TYPE_LEN);
}

void serialize_msg_NF_response(char *buffer, int count_of_fields) {
    buffer = serialize_int(buffer, &(count_of_fields));
}

void deserialize_msg_NF() { // Unused, but leave for the sake of ordering things.
    // empty.
}

void deserialize_msg_NF_response(char *buffer, int *n_field_ids) {
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