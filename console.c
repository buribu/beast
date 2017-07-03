#include <curses.h>
#include <stdlib.h> /* mbstowcs */

#include "beasts.h"
#include "beast.h"

#include "console.h"

#include "faces.h"

void
ConsoleInit()
{
  initscr();
  keypad(stdscr, TRUE);
  cbreak(); /* One char at a time. */
  noecho(); /* Don't echo input. */

  if (has_colors()) {
    start_color();
    /* and stuff */
  }
}

void
ConsoleQuit()
{
  endwin();
}


void
ConsoleLoop(Field *field, Hero *hero)
{
  while (!StopLoop) {
    int c=getch();

    switch (c) {
    case 'j':
    case KEY_UP:
      moveman(UP, field, hero);
      break;
    case 'k':
    case KEY_DOWN:
      moveman(DOWN, field, hero);
      break;
    case 'h':
    case KEY_LEFT:
      moveman(LEFT, field, hero);
      break;
    case 'l':
    case KEY_RIGHT:
      moveman(RIGHT, field, hero);
      break;
    case KEY_HOME:
    case KEY_A1:
      moveman(UP_LEFT, field, hero);
      break;
    case KEY_PPAGE:
    case KEY_A3:
      moveman(UP_RIGHT, field, hero);
      break;
    case KEY_END:
    case KEY_C1:
      moveman(DOWN_LEFT, field, hero);
      break;
    case KEY_NPAGE:
    case KEY_C3:
      moveman(DOWN_RIGHT, field, hero);
      break;
    case 'q':
      return;
      break;
    default:
      break;
    } /* swap (c) */
  } /* while */
} /* ConsoleLoop */

void
ConsoleDrawIcon(int x, int y, Grounds thing)
{
  chtype chstr[2]= { 0,0 };

  if (thing >= BEAST0) {
    switch (beast_type(thing)) {
    case JOE:
      chstr[0]=ACS_LTEE;
      chstr[1]=ACS_RTEE;
      break;
    case PUSHER:
      chstr[0]=chstr[1]=ACS_PLUS;
      break;
    case MOMMA:
      chstr[0]=ACS_LTEE | WA_BOLD;
      chstr[1]=ACS_RTEE | WA_BOLD;
      break;
    case EGG:
      chstr[0]=chstr[1]='*';
      break;
    default:
      g_error("Drawing unknown beast num %d",thing);
    } /* switch(beasttype) */
  } else {
    switch (thing) {
    case EMPTY:
      chstr[0]=chstr[1]=' ';
      break;
    case HERO:
      chstr[0]='<' | WA_BOLD;
      chstr[1]='>' | WA_BOLD;
      break;
    case BLOCK:
      chstr[0]=chstr[1]=ACS_CKBOARD;
      break;
    case WALL:
      chstr[0]=chstr[1]=' ' | WA_REVERSE; /* ACS_BLOCK; */
      break;
    case HOLE:
      /*      chstr[0]='[' | WA_DIM;
	      chstr[1]=']' | WA_DIM; */
      chstr[0]=chstr[1]=ACS_BLOCK | WA_BOLD;
      break;
    default:
      g_error("Drawing unknown %d",thing);
    } /* switch (thing) */
  } /* endif beast */

  mvaddchnstr(y+1, x*2+1, chstr, 2);
}

void
ConsoleRefresh()
{
  refresh();
}

void
ConsoleFieldSize(int *x, int *y)
{
  *x=(COLS-2)/2;
  *y=LINES-4;
}
