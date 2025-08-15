#include <stdio.h>
#include <stdlib.h>

/*
HEADER FILES
*/
#include "classes/templates.h"  /* Template Class */
#include "utils/consoleutils.h" /* Console Functions */
#include "utils/input.h"        /* Input Functions */
#include "app/menus.h"          /* Game Menus */
#include "app/game.h"           /* Game Functions */

/*
 * Templates (un)thankfully had to be hardcoded,
 * I could just use sizeof but fuck you.
 */
#define TEMPLATE_COUNT 5

/*
 * Maximum and minimum width you can set if you're
 * setting up a custom game
 */
#define MIN_WIDTH 10
#define MAX_WIDTH 60

/*
 * Maximum and minimum height you can set if you're
 * setting up a custom game
 */
#define MIN_HEIGHT 10
#define MAX_HEIGHT 40

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

  /* Template definitions */
  Template templates[TEMPLATE_COUNT];

  /* Template init takes: Template Pointer, Template Name, Width, Height, Bomb Amount */
  /* Template colors takes: Template Pointer, Foreground Color, Background Color */

  template_init(&templates[0], "Easy", 10, 10, 10);
  template_colors(&templates[0], CC_BLUE, 0);

  template_init(&templates[1], "Medium", 16, 16, 40);
  template_colors(&templates[1], CC_GREEN, 0);

  template_init(&templates[2], "Hard", 30, 16, 99);
  template_colors(&templates[2], CC_YELLOW, 0);

  template_init(&templates[3], "Expert", 36, 20, 165);
  template_colors(&templates[3], CC_RED, 0);

  template_init(&templates[4], "Master", 36, 30, 252);
  template_colors(&templates[4], CC_WHITE, CC_RED);

  /* Start the program */
  clear_screen();

  /* Program loop */
  while (true)
  {
    /*
    Main Menu
    */
    main_menu();
    int32_t option = read_int("> ");

    bool trigger_exit = false;

    /* Depending on the option, let's do something different */
    switch (option)
    {

    /* Case 1: Let's go for templates */
    case (1):
    {
      /* Display the current templates */
      template_menu(templates, TEMPLATE_COUNT);
      /* Get the user option */
      int32_t template = read_int("> ");

      /* If it's out of bounds, let him know and go back */
      if (template <= 0 || template > TEMPLATE_COUNT)
      {
        printf("The template doesn't exist...");
        fflush(stdout);

        csleep(2);
        break;
      }
      /* Else, let's start a game with the template */
      else
      {
        console_foreground_set(CC_BLUE);
        printf("Depending on your terminal's size, it is possible the game doesn't fit properly on the screen. If this does happen, try to resize and press R to refresh the screen"); /* Print print print */

        fflush(stdout); /* For some reason stuff doesn't show up so I gotta force it to */

        csleep(3.5);
        start_template_game(&templates[template - 1]);
      }
      break;
    }
    /* Case 2: Let's go for a custom game */
    case (2):
    {
      custom_menu();
      int32_t width, height, bomb_amount;

      /* Now, all of these whiles serve the same purpose, hold the user hostage until they do what they're told to (good boy) */
      while (true)
      {
        width = read_int("| Input the game width: ");
        if (width < MIN_WIDTH || width > MAX_WIDTH)
          printf("Invalid width! (Accepted range: %d-%d)\n", MIN_WIDTH, MAX_WIDTH);
        else
          break;
      }

      while (true)
      {
        height = read_int("| Input the game height: ");
        if (height < MIN_HEIGHT || height > MAX_HEIGHT)
          printf("Invalid height! (Accepted range: %d-%d)\n", MIN_HEIGHT, MAX_HEIGHT);
        else
          break;
      }

      while (true)
      {
        bomb_amount = read_int("| Input the bomb amount: ");
        if (bomb_amount < 1 || bomb_amount > width * height - 1)
          printf("Invalid bomb amount! (Accepted range: %d-%d)\n", 1, width * height - 1);
        else
          break;
      }

      /* Start the game let's GOOO */
      start_custom_game(width, height, bomb_amount);
      break;
          /* Read from th*/}
    /* Case 3: Let's get the fuck out of here */
    case (3):
    {
      trigger_exit = true;
      break;
    }
    }

    /* Exit flag */
    if (trigger_exit)
      break;
  }

  /* Reset the consol color before leaving */
  console_color_reset();
  return 0;
}
