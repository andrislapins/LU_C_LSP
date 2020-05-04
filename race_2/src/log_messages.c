#include "../include/common.h"
#include "../include/log_messages.h"

#define TS_BUF_LENGTH 32

static uint64_t log_count = 0;

/* Log header functions */

void log_time_header(FILE *fp) {
    time_t time_val;
    struct tm *tm_info;
    char timestamp[TS_BUF_LENGTH];
    char datestamp[TS_BUF_LENGTH];

    memset(timestamp, '\0', TS_BUF_LENGTH);
    memset(datestamp, '\0', TS_BUF_LENGTH);

    time_val = time(NULL);
    tm_info = localtime(&time_val);

    strftime(datestamp, TS_BUF_LENGTH, "%F (%a)", tm_info);
    strftime(timestamp, TS_BUF_LENGTH, "%H:%M:%S", tm_info);

    fprintf(fp, "%s %li: ", ANSI_GREEN, ++log_count);
    fprintf(fp, "[%s, ", datestamp);
    fprintf(fp, "%s]%s ", timestamp, ANSI_RESET_ALL);
}

void log_client_info(FILE *fp, client_t *client) {
    fprintf(
        fp, "%s%s%s(ID:%d)", 
        ANSI_YELLOW, from_who(client), ANSI_GREEN, client->player->ID
    );
    fprintf(fp, "(%s)%s\n", 
        client->ip, ANSI_RESET_ALL
    );
}

/* General log functions */

void log_pass_generated(FILE *fp, client_t *client) {
    log_time_header(fp);
    log_client_info(fp, client);
    fprintf(
        fp, 
        "Generated a password - %s\n",
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

void log_lost_connection(FILE *fp, client_t *client) {
    log_time_header(fp);
    log_client_info(fp, client);
    fprintf(
        fp, 
        "Lost connection with %s\n", 
        from_who(client)
    );
}

/* Logs for communication functions */

void log_create_game_response(FILE *fp, client_t *client) {
    log_time_header(fp);
    log_client_info(fp, client);
    fprintf(
        fp, 
        "Created a game=%s(ID:%d) on field(ID:%d)\n",
        client->game->game_h->name, client->game->ID, client->game->track->field->ID
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

void log_field_info_response(FILE *fp, client_t *client, int chosen_field_id) {
    log_time_header(fp);
    log_client_info(fp, client);
    fprintf(
        fp,
        "Sent more info about field - %d\n", 
        chosen_field_id
    );
}

void log_game_info_response(FILE *fp, client_t *client, int chosen_game_id) {
    log_time_header(fp);
    log_client_info(fp, client);
    fprintf(
        fp,
        "Sent more info about game - %d\n", 
        chosen_game_id
    );
}


void log_list_games_response(FILE *fp, client_t *client) {
    log_time_header(fp);
    log_client_info(fp, client);
    fprintf(
        fp,
        "Sent the count of games and game IDs\n"
    );
}

/* Logs when deletion of instances occurs */

void log_remove_client(FILE *fp, client_t *client) {
    log_time_header(fp);
    fprintf(
        fp,
        "Deleted client - %s\n",
        from_who(client)
    );
}

void log_remove_game(FILE *fp, char *name) {
    log_time_header(fp);
    fprintf(
        fp,
        "Deleted game - %s\n",
        name
    );
}

void log_remove_track(FILE *fp, char *name) {
    log_time_header(fp);
    fprintf(
        fp,
        "Deleted track - %s\n",
        name
    );
}

/* Log messages for client-side */

void log_received_CG_msg(FILE *fp, char *msg_type, client_t *client) {
    log_time_header(fp);
    fprintf(
        fp,
        "%sReceived%s:\n",
        ANSI_YELLOW, ANSI_RESET_ALL
    );
    fprintf(
        fp,
        "%sType%s: %s\n",
        ANSI_GREEN, ANSI_RESET_ALL,
        msg_type
    );
    fprintf(
        fp,
        "%sGame ID%s: %d\n",
        ANSI_GREEN, ANSI_RESET_ALL,
        client->game->ID
    );
    fprintf(
        fp,
        "%sPlayer ID%s: %d\n",
        ANSI_GREEN, ANSI_RESET_ALL,
        client->player->ID
    );
    fprintf(
        fp,
        "%sPassword%s: %s\n",
        ANSI_GREEN, ANSI_RESET_ALL,
        client->password
    );
}

void log_received_FI_msg(FILE *fp, char *msg_type, client_t *client) {
    log_time_header(fp);
    fprintf(
        fp,
        "%sReceived%s:\n",
        ANSI_YELLOW, ANSI_RESET_ALL
    );
    fprintf(
        fp,
        "%sType%s: %s\n",
        ANSI_GREEN, ANSI_RESET_ALL,
        msg_type
    );
    fprintf(
        fp,
        "%sField ID%s: %d\n", 
        ANSI_GREEN, ANSI_RESET_ALL, 
        client->game->track->field->ID
    );
    fprintf(
        fp,
        "%sField name%s: %s\n",
        ANSI_GREEN, ANSI_RESET_ALL, 
        client->game->track->field->name
    );
    fprintf(
        fp, 
        "%sField width%s: %d\n", 
        ANSI_GREEN, ANSI_RESET_ALL, 
        client->game->track->field->Width
    );
    fprintf(
        fp, 
        "%sField height%s: %d\n", 
        ANSI_GREEN, ANSI_RESET_ALL, 
        client->game->track->field->Height
    );
    fprintf(
        fp, 
        "%sStart line beggining%s x: %f, y: %f\n",
        ANSI_GREEN, ANSI_RESET_ALL, 
        client->game->track->start_line->beggining.x, 
        client->game->track->start_line->beggining.y
    );
    fprintf(
        fp, 
        "%sStart line end%s x: %f, y: %f\n",
        ANSI_GREEN, ANSI_RESET_ALL,
        client->game->track->start_line->end.x, 
        client->game->track->start_line->end.y
    );
    fprintf(
        fp, 
        "%sMain line beggining%s x: %f, y: %f\n", 
        ANSI_GREEN, ANSI_RESET_ALL,
        client->game->track->main_line->beggining.x, 
        client->game->track->main_line->beggining.y
    );
    fprintf(
        fp, 
        "%sMain line end%s x: %f, y: %f\n",
        ANSI_GREEN, ANSI_RESET_ALL,
        client->game->track->main_line->end.x, 
        client->game->track->main_line->end.y
    );
    fprintf(
        fp, 
        "%sNumber of extra lines%s: %d\n", 
        ANSI_GREEN, ANSI_RESET_ALL,
        client->game->track->n_extra_lines
    );
}

void log_received_LI_msg(FILE *fp, char *msg_type, int n_games, int *gid_arr) {
    log_time_header(fp);
    fprintf(
        fp,
        "%sReceived%s:\n",
        ANSI_YELLOW, ANSI_RESET_ALL
    );
    fprintf(
        fp,
        "%sType%s: %s\n",
        ANSI_GREEN, ANSI_RESET_ALL,
        msg_type
    );

    // Check the value of count of games.
    if (n_games == 1) {
        fprintf(
            fp,
            "%sThere is currently %s%d%s game on the server%s\n",
            ANSI_GREEN, ANSI_YELLOW,
            n_games,
            ANSI_GREEN, ANSI_RESET_ALL
        );
    } else {
        fprintf(
            fp,
            "%sThere are currently %s%d%s games on the server%s\n",
            ANSI_GREEN, ANSI_YELLOW,
            n_games,
            ANSI_GREEN, ANSI_RESET_ALL
        );
    }

    // Print those game IDs.
    for(int i = 0; i < n_games; i++) {
        fprintf(
            fp,
            "%sGID%s: %d\n",
            ANSI_GREEN, ANSI_RESET_ALL,
            gid_arr[i]
        );
    }
}

void log_received_GI_msg(
    FILE *fp, char *msg_type, client_t *client,
    int g_client_count, struct Player_info ***p
) {
    log_time_header(fp);
    fprintf(
        fp,
        "%sReceived%s:\n",
        ANSI_YELLOW, ANSI_RESET_ALL
    );
    fprintf(
        fp,
        "%sType%s: %s\n",
        ANSI_GREEN, ANSI_RESET_ALL,
        msg_type
    );

    // Displaying game header info.
    fprintf(
        fp,
        "%sGame status%s: %d\n",
        ANSI_GREEN, ANSI_RESET_ALL,
        client->game->game_h->status
    );
    fprintf(
        fp,
        "%sGame name%s: %s\n",
        ANSI_GREEN, ANSI_RESET_ALL,
        client->game->game_h->name
    );
    fprintf(
        fp,
        "%sGame winner ID%s: %d\n",
        ANSI_GREEN, ANSI_RESET_ALL,
        client->game->game_h->WinnerPlayerID
    );
    fprintf(
        fp,
        "%sPlayer count%s: %d\n\n",
        ANSI_GREEN, ANSI_RESET_ALL,
        g_client_count
    );

    // Displaying player info one by one.
    for(int i = 0; i < g_client_count; i++) {
        fprintf(
            fp,
            "%sPlayer ID%s: %d\n",
            ANSI_GREEN, ANSI_RESET_ALL,
            (*p)[i]->ID
        );
        fprintf(
            fp,
            "%sPlayer Name%s: %s%s%s\n",
            ANSI_GREEN, ANSI_RESET_ALL, ANSI_YELLOW,
            (*p)[i]->name,
            ANSI_YELLOW
        );
        fprintf(
            fp,
            "%sPlayer positon%s x: %f, y: %f\n",
            ANSI_GREEN, ANSI_RESET_ALL,
            (*p)[i]->position.x, (*p)[i]->position.y
        );
        fprintf(
            fp,
            "%sPlayer angle%s: %f\n",
            ANSI_GREEN, ANSI_RESET_ALL,
            (*p)[i]->angle
        );
        fprintf(
            fp,
            "%sPlayer speed%s: %f\n",
            ANSI_GREEN, ANSI_RESET_ALL,
            (*p)[i]->speed
        );
        fprintf(
            fp,
            "%sPlayer acceleration%s: %f\n",
            ANSI_GREEN, ANSI_RESET_ALL,
            (*p)[i]->acceleration
        );
        fprintf(
            fp,
            "%sPlayer laps%s: %d\n\n",
            ANSI_GREEN, ANSI_RESET_ALL,
            (*p)[i]->laps
        );
    }

    // Displaying info about the field.
    fprintf(
        fp,
        "%sField ID%s: %d\n",
        ANSI_GREEN, ANSI_RESET_ALL,
        client->game->track->field->ID
    );
    fprintf(
        fp,
        "%sField name%s: %s\n",
        ANSI_GREEN, ANSI_RESET_ALL,
        client->game->track->field->name
    );
    fprintf(
        fp,
        "%sField width%s: %d\n",
        ANSI_GREEN, ANSI_RESET_ALL,
        client->game->track->field->Width
    );
    fprintf(
        fp,
        "%sField heigth%s: %d\n",
        ANSI_GREEN, ANSI_RESET_ALL,
        client->game->track->field->Height
    );
}
