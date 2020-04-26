#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <unistd.h>

typedef struct {
    int xLoc, yLoc, xMax, yMax;
    char character;
    WINDOW *curwin;
} Player_t;

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

int main(int argc, char **argv)
{
    initscr();
    noecho();
    cbreak();
    curs_set(FALSE); // Do not display a cursor.

    int ymax, xmax;
    getmaxyx(stdscr, ymax, xmax);

    WINDOW *play_win = newwin(20, xmax-20, (ymax/2)-10, 10);
    box(play_win, 0, 0);
    mvwprintw(stdscr, 24, (xmax/2)-8, "Press x to quit");
    refresh();
    wrefresh(play_win);
    keypad(play_win, TRUE);

    Player_t *p = init_player(play_win, 1, 1, '@');

    do {
        display(p);
        wrefresh(play_win);
    } while(getmv(p) != 'x');

    endwin();
    return 0;
}