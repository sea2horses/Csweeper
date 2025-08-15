#ifndef MINEFIELD_H
#define MINEFIELD_H

#include <stdbool.h>
#include <stdint.h>

/* Minefield struct definition */
typedef struct
{
  bool has_bomb;
  bool is_flagged;
  bool is_mined;
  uint8_t bomb_amount;
} Minefield;

void init_minefield(Minefield *minefield);

#endif /* MINEFIELD_H */