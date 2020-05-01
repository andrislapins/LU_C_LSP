#include "../include/common.h"
#include "../include/log_messages.h"

#define TS_BUF_LENGTH 32

/* Log header functions */

void log_time_header(FILE *fp) {
    static uint64_t log_count = 0;

    time_t time_val;
    struct tm *tm_info;
    char timestamp[TS_BUF_LENGTH];
    char datestamp[TS_BUF_LENGTH];

    time_val = time(NULL);
    tm_info = localtime(&time_val);

    strftime(datestamp, TS_BUF_LENGTH, "%F (%a)", tm_info);
    strftime(timestamp, TS_BUF_LENGTH, "%H:%M:%S", tm_info);

    fprintf(fp, ANSI_BOLD);
    fprintf(fp, "%li: ", ++log_count);
    fprintf(fp, "[%s, ", datestamp);
    fprintf(fp, "%s] ", timestamp);
    fprintf(fp, ANSI_NORMAL);   
}

void log_client_info(FILE *fp, client_t *client) {
    fprintf(fp, ANSI_BOLD);
    fprintf(fp, "%s(%d)", from_who(client), client->player->ID);
    fprintf(fp, "(%s) - ", client->ip);
    fprintf(fp, ANSI_NORMAL);   
}

/* General log functions */

void log_pass_generated(FILE *fp, client_t *client) {
    log_time_header(fp);
    log_client_info(fp, client);
    fprintf(
        fp, 
        "Generated a password - %s",
        client->password
    );
}

void log_recvd_msg_type(FILE *fp, client_t *client, char *msg_type) {
    log_time_header(fp);
    log_client_info(fp, client);
    fprintf(
        fp,
        "Received message of type - %s\n",
        msg_type
    );
}

/* Logs for communication functions */

void log_create_game_response(FILE *fp, client_t *client) {
    log_time_header(fp);
    log_client_info(fp, client);
    fprintf(
        fp, 
        "Created a game(ID:%d) on field(ID:%d)",
        client->game->ID, client->game->track->field->ID
    );
}

void log_get_number_of_fields_response(FILE *fp, client_t *client) {
    log_time_header(fp);
    log_client_info(fp, client);
    fprintf(
        fp,
        "Sent the count of available fields\n"
    );
}

void log_field_info_response(FILE *fp, client_t *client, int *chosen_field_id) {
    log_time_header(fp);
    log_client_info(fp, client);
    fprintf(
        fp,
        "Sent more info about field - %d\n", 
        *chosen_field_id
    );
}

void log_list_games_response(FILE *fp, client_t *client) {
    log_time_header(fp);
    log_client_info(fp, client);
    fprintf(
        fp,
        "Sent the count of fields\n"
    );
}