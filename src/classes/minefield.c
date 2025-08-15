#include "minefield.h"

/* Just initialize it */
void init_minefield(Minefield *minefield)
{
  minefield->bomb_amount = 0;
  minefield->has_bomb = false;
  minefield->is_flagged = false;
  minefield->is_mined = false;
}