/* Sorts of beasts */
typedef enum {
  JOE,
  PUSHER,
  MOMMA,
  EGG,
  DEAD
} BeastieType;

#define BEASTIETYPES 4 /* Types of live beasties, that is. */



void spawn_beasties(Field *fieldPtr, 
		    int numbeasties[BEASTIETYPES]);

void kill_beastie(int beastnum);

BeastieType beast_type(int beastnum);

void init_timer(long usecs);
void start_timer();
