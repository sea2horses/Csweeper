#include <stdbool.h>

#include "vec.h"

bool vec_cmpr(Vec2 vec_a, Vec2 vec_b)
{
  return vec_a.x == vec_b.x && vec_a.y == vec_b.y;
}