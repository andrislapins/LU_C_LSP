#ifndef _SERIALIZATION_H_
#define _SERIALIZATION_H_

#include "common.h"
#include "player.h"

unsigned char * serialize_int(unsigned char *buffer, int value);
unsigned char * serialize_char(unsigned char *buffer, char value);
unsigned char * serialize_player(unsigned char *buffer, Player_t *p);
// unsigned char * deserialize_int(unsigned char *buffer, int *value);
unsigned char * deserialize_int(unsigned char *buffer, int *value);
unsigned char * deserialize_char(unsigned char *buffer, char *value);
void deserialize_player(unsigned char *buffer, Player_t *p);
int send_player(int socket, Player_t *p);

#endif