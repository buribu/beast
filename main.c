/* Beasts.
   A game from my youth. */

/* Includes */

#include <time.h>   /* time (for seeding) */

#include "beasts.h"
#include "beast.h" 

#ifndef NO_CONSOLE_INTERFACE
#include "console.h"
#endif

/*************** Defines *****************/



    
/***** Prototypes ******/

static void init_grounds(Field *field, 
			 float blocks, float walls, float holes);

static void draw_field(Field *field);

/* extern DrawIconFunc ConsoleDrawIcon; */

/***** Defines ********/

static DrawIconFunc (*draw_icon);
static void (*field_refresh)();

/*************** Functions *********************/

int
main()
{
  int x, y;
  int level=0;
  Field field;
  int numbeasts[BEASTIETYPES]={6,0,0,0};
  Hero hero;
  
  GlobalHeroPtr=&hero;
  GlobalFieldPtr=&field;

  /* Determine interface mode (slang, gnome). */

  ConsoleInit();
  draw_icon=&ConsoleDrawIcon;
  field_refresh=&ConsoleRefresh;

  /* Get screen size. */

  /* Calculate playing field size. */

  ConsoleFieldSize(&x,&y);

  srand(time(NULL));
  field.map=g_new(Grounds,x*y);
  field.x=x;
  field.y=y;


  init_timer(1000000/6);
  /* Establish all the callbacks. */
  

    level++;
    init_grounds(&field,
	       0.25, 0.001, 0.001);
    spawn_beasties(&field, numbeasts);
    place_thing(HERO, &field, &hero.x, &hero.y);

    draw_field(&field);
    /* Wait for a key or something, and...*/

    /* Action! */
    StopLoop=FALSE;
    start_timer();
    ConsoleLoop(&field, &hero);

    /* Hopefully everything is event based 
       and we sleep for a while. */
    
    /* Something caused the level to end.
       Who won?  */

  ConsoleQuit();
  
  exit(0);
}

void
endlevel(char *msg)
{
  StopLoop=TRUE;
}

static void 
init_grounds(Field *fieldPtr,
	   float blocks, float walls, float holes)
{
  int xx, yy;
  int yoff;

  const int x=fieldPtr->x, y=fieldPtr->y;
  Grounds *field=fieldPtr->map;

  float randfoo;

  /* Make sure outer walls are intact. */
  for(xx=0;xx<x;xx++) {
    field[xx]=WALL;
    field[(y-1)*x+xx]=WALL;
  }
  for(yy=0;yy<y;yy++) {
    field[yy*x]=WALL;
    field[yy*x+(x-1)]=WALL;
  }

  walls+=blocks; /* See below.  You'll figure it out. */
  holes+=walls;

  if(holes>=1)
    g_error("Playing field overfull with junk.");

  /* Drop some random blocks, walls, holes. */
  for(yy=1;yy<y-1;yy++) {
    yoff=yy*x;
    for(xx=1;xx<x-1;xx++) {
      randfoo=float_rand();
      if (randfoo<blocks)
	field[yoff+xx] = BLOCK;
      else if (randfoo<walls)
	field[yoff+xx] = WALL;
      else if (randfoo<holes)
	field[yoff+xx] = HOLE;
    } /* next xx */
  } /* next yy */

} /* init_grounds */

gboolean
place_thing(Grounds thing, Field *fieldPtr, int *thing_x, int *thing_y)
{
  int xx=1, yy=1;

  const int x=fieldPtr->x, y=fieldPtr->y;
  Grounds *field=fieldPtr->map;

  const int RELATOR_SANITY=x*y*2;

  int refugee=0;

  do {
    yy=int_rand(1,y-2);
    xx=int_rand(1,x-2);

    if (refugee++ > RELATOR_SANITY) 
      return FALSE;
  } while(field[yy*x+xx] != EMPTY);

  *thing_x = xx;
  *thing_y = yy;
  field[yy*x+xx]=thing; 

  return TRUE;
}

/* High-level drawing functions. */

static void
draw_field(Field *field)
{
  int yy, xx;
  int yoff;

  for(yy=0; yy < field->y; yy++) {
    yoff=yy*field->x;
    for(xx=0; xx < field->x; xx++) {
      if(field->map[yoff+xx] != EMPTY) {
      /*	(*draw_icon)(xx,yy,field->map[yoff+xx]); */
	ConsoleDrawIcon(xx,yy,field->map[yoff+xx]);
      } /* endif not empty */
    } /* next xx */
  } /* next yy */
  (*field_refresh)();
}

/*********** Block pushing ****************/
/* Returns TRUE if blocks moved. */

static gboolean
push_blocks(Field *field, int x, int y, int step)
{
  int stepped=0;
  int yoff;

  int endpos, end_x, end_y, victim;

  yoff=y*field->x;

  stepped=step;
  while(field->map[yoff + x + stepped] == BLOCK) {
    stepped+=step;
  }

  endpos = yoff + x + stepped;
  victim=field->map[endpos];

  if (victim==WALL) {
    /* Nothing moves. */
    return FALSE;
  }

  if (victim >= BEAST0) {
    if (field->map[endpos + step] == EMPTY)
      return FALSE;
    else
      kill_beastie(victim - BEAST0);        
  }

  field->map[yoff + x + step] = EMPTY; /* Push away first block */

  if (victim != HOLE) {
    field->map[endpos] = BLOCK;   /* and put block on end. */
  } else {
    /* A hole is still a hole. */
    return TRUE;
  }

  end_y = (yoff + x + stepped) / field->x;
  end_x = (yoff + x + stepped) % field->x;

  ConsoleDrawIcon(end_x, end_y, BLOCK);
  /* (*draw_icon)(end_x,end_y,BLOCK); */

  return TRUE;
}

void
moveman(Direction thatway, Field *field, Hero *hero)
{
  int step=dir_to_step(thatway, field->x);
  int oldpos=field->x*hero->y + hero->x;
  int newpos=oldpos+step;

  if (field->map[newpos]==EMPTY || 
      ((field->map[newpos]==BLOCK) &&
       push_blocks(field, hero->x,hero->y,step))) {
    field->map[oldpos]=EMPTY;
    field->map[newpos]=HERO;
  } else {
    return;
  }


  /* (*draw_icon)(hero->x, hero->y, EMPTY); */
  ConsoleDrawIcon(hero->x, hero->y, EMPTY);

  switch (thatway) {
  case UP_LEFT:    
    hero->x--;
    hero->y--;
    break;
  case UP:
    hero->y--;
    break;
  case UP_RIGHT:  
    hero->x++;
    hero->y--;
    break;
  case RIGHT:
    hero->x++;
    break;
  case DOWN_RIGHT: 
    hero->y++;
    hero->x++;
    break;
  case DOWN:
    hero->y++;
    break;
  case DOWN_LEFT:
    hero->y++;
    hero->x--;
    break;
  case LEFT: 
    hero->x--;
    break;

  default: break;
  }

  ConsoleDrawIcon(hero->x, hero->y, HERO);
  /* (*draw_icon)(hero->x, hero->y, HERO); */

  (*field_refresh)();

}

int
dir_to_step(Direction thatway, int rowspan)
{
  int step;

  switch(thatway) {
  case UP_LEFT:    step=-rowspan-1; break;
  case UP:         step=-rowspan;   break;
  case UP_RIGHT:   step=-rowspan+1; break;
  case RIGHT:      step=1;    break;
  case DOWN_RIGHT: step=+rowspan+1; break;
  case DOWN:       step=rowspan;    break;
  case DOWN_LEFT:  step=+rowspan-1; break;
  case LEFT:       step=-1;   break;

  default: g_error("Bad direction %d",thatway);
  }

  return step;
}

