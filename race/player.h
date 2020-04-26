#include "common.h"

#ifndef _PLAYER_H_
#define _PLAYER_H_


typedef struct {
    int pid;
    int xLoc, yLoc, xMax, yMax;
    char character;
    WINDOW *curwin;
} Player_t;

Player_t* init_player(WINDOW *win, int y, int x, char c);
void mvup(Player_t *p);
void mvdown(Player_t *p);
void mvleft(Player_t *p);
void mvright(Player_t *p);
int getmv(Player_t *p);
void display(Player_t *p);

#endif