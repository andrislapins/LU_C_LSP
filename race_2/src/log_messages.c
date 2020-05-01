#include "../include/common.h"
#include "../include/log_messages.h"

#define TS_BUF_LENGTH 32

void log_header(FILE *fp, client_t *client) {
    static uint64_t log_count = 0;

    time_t time_val;
    char timestamp[TS_BUF_LENGTH];
    char datestamp[TS_BUF_LENGTH];
    struct tm *tm_info;

    time_val = time(NULL);
    tm_info = localtime(&time_val);

    strftime(datestamp, TS_BUF_LENGTH, "%F (%a)", tm_info);
    strftime(timestamp, TS_BUF_LENGTH, "%H:%M:%S", tm_info);

    fprintf(fp, "%lli: ", ++log_count);
    fprintf(fp, "[%s, ", datestamp);
    fprintf(fp, "%s] ", timestamp);        
    fprintf(fp, "%s(%d)", from_who(client), client->player->ID);
    fprintf(fp, "(%s) - ", client->ip);
}

void log_create_game_response(FILE *fp, client_t *client) {
    log_header(fp, client);
    fprintf(
        fp, 
        "Created a game(ID:%d) on field(ID:%d)",
        client->game->ID, client->game->field->field->ID
    );
}

void log_pass_generated(FILE *fp, client_t *client) {
    log_header(fp, client);
    fprintf(
        fp, 
        "Generated a password - %s",
        client->password
    );
}