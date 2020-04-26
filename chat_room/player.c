#include "player.h"

Player_t* init_player(WINDOW *win, int y, int x, char c)
{
    Player_t *p = malloc(sizeof(Player_t));
    p->curwin = win;
    p->yLoc = y;
    p->xLoc = x;
    getmaxyx(p->curwin, p->yMax, p->xMax);
    keypad(p->curwin, 1);
    p->character = c;
}

void mvup(Player_t *p)
{
    mvwaddch(p->curwin, p->yLoc, p->xLoc, ' ');

    p->yLoc--;
    if (p->yLoc < 1) {
        p->yLoc = 1;
    }
}

void mvdown(Player_t *p)
{
    mvwaddch(p->curwin, p->yLoc, p->xLoc, ' ');

    p->yLoc++;
    if (p->yLoc > p->yMax - 2) {
        p->yLoc = p->yMax - 2;
    }
}


void mvleft(Player_t *p)
{
    mvwaddch(p->curwin, p->yLoc, p->xLoc, ' ');

    p->xLoc--;
    if (p->xLoc < 1) {
        p->xLoc = 1;
    }
}

void mvright(Player_t *p)
{
    mvwaddch(p->curwin, p->yLoc, p->xLoc, ' ');

    p->xLoc++;
    if (p->xLoc > p->xMax - 2) {
        p->xLoc = p->xMax - 2;
    }
}

int getmv(Player_t *p)
{
    int choice;
    
    choice = wgetch(p->curwin);
    switch(choice) {
        case KEY_UP:
            mvup(p);
            break;
        case KEY_DOWN:
            mvdown(p);
            break;
        case KEY_LEFT:
            mvleft(p);
            break;
        case KEY_RIGHT:
            mvright(p);
            break;
        default:
            break;
    }

    return choice;
}

void display(Player_t *p)
{
    mvwaddch(p->curwin, p->yLoc, p->xLoc, p->character);
}