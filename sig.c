#include "beasts.h"

extern void beastie_dance(Field *field, Hero *hero);

void
beastie_call()
{
  beastie_dance(GlobalFieldPtr, GlobalHeroPtr);
}
