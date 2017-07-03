#include "glib.h"
#include <stdlib.h> /* rand, srand */

typedef enum {
  EMPTY = 0,
  BLOCK,
  WALL,
  HOLE,
  HERO,
  BEAST0 /* The rest are beasts. */
} Grounds;

typedef struct {
  Grounds *map;
  int x;
  int y;
} Field;

typedef struct {
  int x;
  int y;
  int lives;
} Hero;

typedef enum {
  UP,
  DOWN,
  LEFT,
  RIGHT,
  UP_LEFT,
  UP_RIGHT,
  DOWN_LEFT,
  DOWN_RIGHT
} Direction;

typedef void (*DrawIconFunc) (int x, int y, Grounds thing);

/* in main.c */
gboolean place_thing(Grounds thing, 
		     Field *fieldPtr, 
		     int *thing_x, 
		     int *thing_y);

void moveman(Direction thatway, 
	     Field *field,
	     Hero *hero);

int dir_to_step(Direction thatway, 
		int rowspan);

Hero  *GlobalHeroPtr;
Field *GlobalFieldPtr;
gboolean StopLoop;

#define float_rand()  ((float)rand()/(float)RAND_MAX)
#define int_rand(N,X) ((int)(((double)rand()/(double)RAND_MAX)*((X)-(N)+1)+(N)))
