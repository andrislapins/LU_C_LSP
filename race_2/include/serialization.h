#ifndef _SERIALIZATION_H_
#define _SERIALIZATION_H_

#include "common.h"
#include "protocol.h"

char *serialize_int(char *buffer, int *value);
char* deserialize_int(char *buffer, int *value);

char *serialize_float(char *buffer, float *value);
char *deserialize_float(char *buffer, float *value);

char *serialize_string(char *buffer, char *value, int len);
char *deserialize_string(char *buffer, char *value, int len);

void serialize_field(char *buffer, struct Field *field);
void deserialize_field(char *buffer, struct Field *field);

void serialize_msg_CG(char *buffer, char *msg_type, char *player_name, char *game_name, int field_id);
void deserialize_msg_CG(char *buffer, client_t *client);
void serialize_msg_CG_response(char *buffer, client_t *client);
void deserialize_msg_CG_response(char *buffer, client_t *client);

void serialize_msg_NF(char *buffer, char *msg_type);
void deserialize_msg_NF(); // Unused, but leave for the sake of ordering things.
void serialize_msg_NF_response(char *buffer, int count_of_fields);
void deserialize_msg_NF_response(char *buffer, int *n_field_ids);

void serialize_msg_FI(char *buffer, char *msg_type, int chose);
void deserialize_msg_FI(char *buffer, int *chose);
void serialize_msg_FI_response(char *buffer,field_t *myfield);
void deserialize_msg_FI_response(char *buffer,field_t *myfield);

#endif