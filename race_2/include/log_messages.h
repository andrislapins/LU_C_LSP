#ifndef _LOG_MESSAGES_H
#define _LOG_MESSAGES_H

#include "common.h"

void log_time_header(FILE *fp);
void log_client_info(FILE *fp, client_t *client);

void log_pass_generated(FILE *fp, client_t *client);
void log_recvd_msg_type(FILE *fp, client_t *client, char *msg_type);

void log_create_game_response(FILE *fp, client_t *client);
void log_get_number_of_fields_response(FILE *fp, client_t *client);
void log_field_info_response(FILE *fp, client_t *client, int *chosen_field_id);
void log_list_games_response(FILE *fp, client_t *client);

void log_remove_client(FILE *fp, char *name);
void log_remove_game(FILE *fp, char *name);
void log_remove_track(FILE *fp, char *name);

#endif