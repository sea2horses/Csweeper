#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "utils/input.h"

int main()
{

  /*
  Activate and register for deactivate special functions for the console
  to activate non-blocking input (necessary for UNIX)
  */
#ifndef _WIN32
  init_term();
  atexit(reset_term);
#endif
  printf("Press keys!\n");

  while (true)
  {
    vkey_t key = get_key();
    if (key != VK_NONE)
      printf("Key: '%c', ord: %d\n", key, key);

    if (key == VK_ENTER)
      printf("you pressed enter.\n");
    if (key == VK_ESCAPE)
      break;
  }
}