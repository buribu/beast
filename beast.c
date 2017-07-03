#include <sys/time.h> /* setitimer */
#include <signal.h>   /* signal */
#include <math.h>     /* copysign */

#include "beasts.h"
#include "beast.h"
#include "console.h" /* FIXME: should be draw.h */

/* Structure of a beast */
typedef struct {
  BeastieType species;
  int x;
  int y;
  int phobic;
  int preggers;
  int lastmoved;
} Beastie;

static Beastie *beast;
static int totalbeasts;
static int livebeasts;

extern void beastie_call();

static void move_beastie(int bnum, int turnnum, Field *field, Hero *hero);

static void boom_beastie(int bnum, Field *field);

void
spawn_beasties(Field *fieldPtr, int numbeasties[BEASTIETYPES])
{

  int species, sibling;


  int beast_index=BEAST0;
  Beastie *beastPtr;

  /************** Out with the beasties! ********************/

  for(species=0; species < BEASTIETYPES; species++)
    totalbeasts+=numbeasties[species];

  livebeasts=totalbeasts;
  /* Make a new beastielist. */

  g_free(beast);
  beast = g_new(Beastie,totalbeasts);
  beastPtr = beast;

  for(species=0; species < BEASTIETYPES; species++) {

    for(sibling=0; sibling < numbeasties[species]; sibling++) {

      beastPtr->species = species;

      switch(species) {
      default:
	beastPtr->preggers=-1;
	beastPtr->phobic=0;
      }

      place_thing(beast_index,
		  fieldPtr,
		  &beastPtr->x,
		  &beastPtr->y);

      beastPtr++;
      beast_index++;
    } /* Next sibling. */

  } /* next beast type. */
}

void
kill_beastie(int beastnum)
{
  beast[beastnum].species=DEAD;
  /* gory sound effects */

  livebeasts--;
  if(livebeasts==0)
    endlevel("All beasts dead.");

  /* score++; */
}

BeastieType
beast_type(int beastnum)
{
  return beast[beastnum-BEAST0].species;
}

/************** Timer for beasties. **************/
void
init_timer(long usecs) {
  struct itimerval foo;

  signal(SIGALRM, SIG_IGN);

  foo.it_interval.tv_sec=
    foo.it_value.tv_sec=0;

  foo.it_interval.tv_usec=
    foo.it_value.tv_usec=usecs;

  setitimer(ITIMER_REAL, &foo, NULL);
}

void 
start_timer()
{
  signal(SIGALRM, beastie_call);
}
/*********** Beastie Behaviour ****************/

void
beastie_dance(Field *field, Hero *hero)
{
  static int i=0;
  static int turnnum=0;
  
  i++;
  i=i % totalbeasts;
  if (i==0)
    turnnum++;
  
  /*  for(i=0;i<totalbeasts;i++) { */
    if (beast[i].species != DEAD)
      move_beastie(i,turnnum, field, hero);

      /*  }  next beast */
  /* (*RefreshField)(); */
  ConsoleRefresh();
} /* beastie_dance */

static void
move_beastie(int bnum, int turnnum, Field *field, Hero *hero)
{
  int bpos, endpos;
  int deltax, deltay;
  int dx=0, dy=0, dx0=0, dy0=0;
  int try=0;
  Direction whichway[8];
  Direction whichbar, fooway, i;
  Grounds flub;
  float x_to_y;
  gboolean go_XY, YX_Failed = FALSE, XY_Failed=FALSE, Trapped=FALSE;

  beast[bnum].lastmoved = turnnum;

  deltax=hero->x - beast[bnum].x;
  if (deltax != 0) 
    dx0=dx=(deltax > 0) ? 1 : -1;

  deltay=hero->y - beast[bnum].y;
  if (deltay != 0)
    dy0=dy=(deltay > 0) ? 1 : -1;

  bpos=beast[bnum].y * field->x + beast[bnum].x;

  endpos=bpos + field->x * dy + dx;
  flub=field->map[endpos];
  if (flub==HERO) {
    endlevel("Hero is beastie snack.");
  }

  if (beast[bnum].phobic > int_rand(50,120)) {
    beast[bnum].species = DEAD;
    boom_beastie(bnum,field);
    return;
  }

  x_to_y= fabs((double)deltax/(double)deltay);
  if ((x_to_y > 2) && beast[bnum].phobic < 10)
    go_XY = TRUE;
  else if ((x_to_y < 0.5) && beast[bnum].phobic < 10)
    go_XY = FALSE;
  else
    go_XY = (rand() < (RAND_MAX / 2)) ? FALSE : TRUE;

  /* If we're scared enough, run randomly. */
  if (beast[bnum].phobic > 20)
    YX_Failed=XY_Failed=TRUE;

  while ((flub != EMPTY) && !Trapped) {

    if ((flub >= BEAST0) && beast[flub - BEAST0].lastmoved < turnnum) {
      /* Ask beastie friend to move first. */
      move_beastie(flub - BEAST0, turnnum, field, hero);
    } else {
      /*********** Beastie Brains ************/
      if ((go_XY || YX_Failed) && !XY_Failed) {
	if (deltay != 0) {
	  if (abs(dy-dy0) < 2) {
	    dy-=dy0;  /* Sacrifice Y movement */
	  } else {
	    XY_Failed=TRUE; /* Method failed. */
	    beast[bnum].phobic += 1; /* Earn paranoia point. */
	    dy=dy0;         /* Reset. */
	  } /* endif (abs(dy-dy0) < 2) */
	} else { /* delaty == 0 */
	  try++;
	  switch(try) {
	  case 1:
	    dy=int_rand(-1,1);
	    break;
	  case 2:
	    dy=-dy;
	    break;
	  case 3:  /* Third strike. */
	  default:
	    XY_Failed=TRUE;
	    beast[bnum].phobic += 1;
	    try=0;
	  } /* end switch(try++) */
	} /* deltay == 0 */

	endpos = bpos + field->x * dy + dx;

      } else if (!YX_Failed) { /* YX */
	if (deltax != 0) {
	  if (abs(dx-dx0) < 2) {
	    dx-=dx0;  /* Sacrifice X movement */
	  } else {
	    YX_Failed=TRUE; /* Method failed. */
	    beast[bnum].phobic += 1; /* Earn paranoia point. */
	    dx=dx0;         /* Reset. */
	  } /* endif (abs(dx-dx0) < 2) */
	} else { /* delatx == 0 */
	  try++;
	  switch(try) {
	  case 1:  /* Pick one. */
	    dx=int_rand(-1,1);
	    break;
	  case 2:  /* Try the other way. */
	    dx=-dx;
	    break;
	  case 3:  /* Third strike. */
	  default:
	    YX_Failed=TRUE;
	    beast[bnum].phobic += 1;
	    try=0;
	  } /* end switch(try++) */
	} /* deltax == 0 */

	endpos=bpos + field->x * dy + dx;

      } else { /* Desperate */

	if (try==0) {	  
	  /* Generate new randomlist */
	  for(i=0;i<8;i++)
	    whichway[i]=i;
	  for(i=0;i<8;i++) {
	    whichbar=int_rand(0,7);
	    fooway=whichway[i];
	    whichway[i]=whichway[whichbar];
	    whichway[whichbar]=fooway;
	  }
	} else if (try>7) {
	  Trapped=TRUE;
	  beast[bnum].phobic += 3; /* It's scary in here! */
	} else {
	  endpos= bpos + dir_to_step(whichway[try],field->x);
	  switch (whichway[try]) {
	  case UP_LEFT:    
	    dx=-1;
	    dy=-1;
	    break;
	  case UP:
	    dx= 0;
	    dy=-1;
	    break;
	  case UP_RIGHT:  
	    dx= 1;
	    dy=-1;
	    break;
	  case RIGHT:
	    dx= 1;
	    dy= 0;
	    break;
	  case DOWN_RIGHT: 
	    dx= 1;
	    dy= 1;
	    break;
	  case DOWN:
	    dx= 0;
	    dy= 1;
	    break;
	  case DOWN_LEFT:
	    dx=-1;
	    dy= 1;
	    break;
	  case LEFT: 
	    dx=-1;
	    dy= 0;
	    break;
	  default: break;
	  } /* switch */
	}
	try++;

      } /* Desperate */
    } /* endif (not beast) */

    flub=field->map[endpos];
  } /* while flub != empty and !Trapped */

  if(Trapped)
    return; /* Nothing left to do if we didn't move. */
  
  field->map[bpos]=EMPTY;
  ConsoleDrawIcon(beast[bnum].x,beast[bnum].y,EMPTY);

  beast[bnum].x+=dx;
  beast[bnum].y+=dy;
  if(beast[bnum].phobic > 0)
    beast[bnum].phobic-=1;

  field->map[endpos]=BEAST0+bnum;
  ConsoleDrawIcon(beast[bnum].x,beast[bnum].y,BEAST0+bnum);
}

static void
boom_beastie(int bnum, Field *field)
{
  int xx, yy, yoff;

  for(yy=beast[bnum].y-1;yy <= beast[bnum].y +1;yy++) {
    yoff=field->x * yy;
    for(xx=beast[bnum].x-1;xx <= beast[bnum].x + 1; xx++) {
      
      if (field->map[yoff+xx] >= BEAST0) {
	kill_beastie(field->map[yoff+xx] - BEAST0);
      }
      switch (field->map[yoff+xx]) {
      case EMPTY:
      case WALL:
      case HOLE:
	break;
      case HERO:
	endlevel("Hero blown to pieces.");
      default:
	ConsoleDrawIcon(xx,yy,EMPTY);
	field->map[yoff+xx]=EMPTY;
      } /* end switch */
    } /* next xx */
  } /* next yy */
} /* boom_beastie */
