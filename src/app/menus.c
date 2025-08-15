#include <stdio.h>

#include "menus.h"
#include "../utils/consoleutils.h"
#include "titles.h"

/*
 * Just a file to print all the menus
 */

void main_menu()
{
  clear_screen();

  console_foreground_set(CC_MAGENTA);
  title_print_game();

  console_foreground_set(CC_YELLOW);
  printf("v 1.0, by @sea2horses\n");

  console_foreground_reset();
  printf("\n\n");
  printf("Would you like to select a template or play a custom game?\n");

  printf("| 1. ");
  console_foreground_set(CC_YELLOW);
  printf("Select a Template (Easy, Hard, Master)\n");

  console_foreground_reset();
  printf("| 2. ");
  console_foreground_set(CC_YELLOW);
  printf("Play a Custom Game\n");

  console_foreground_reset();
  printf("| 3. ");
  console_foreground_set(CC_RED);
  printf("Exit\n");

  console_color_reset();

  printf("\n");
}

void template_menu(Template *templates, uint8_t template_count)
{
  clear_screen();
  console_foreground_set(CC_MAGENTA);
  title_print_game();

  console_foreground_set(CC_YELLOW);
  printf("v 1.0, by @sea2horses\n");

  console_foreground_reset();
  printf("\n\n");
  printf("Select a template: \n");
  for (uint8_t i = 0; i < template_count; i++)
  {
    printf("| %d. ", i + 1);
    if (templates[i].fg_color != 0)
      console_foreground_set(templates[i].fg_color);
    if (templates[i].bg_color != 0)
      console_background_set(templates[i].bg_color);

    printf("%s", templates[i].name);
    console_color_reset();
    printf(" - (%d x %d), %d bombs\n", templates[i].width, templates[i].height, templates[i].bomb_amount);
  }

  printf("\n");
}

void custom_menu()
{
  clear_screen();

  console_foreground_set(CC_MAGENTA);
  title_print_game();

  console_foreground_set(CC_YELLOW);
  printf("v 1.0, by @sea2horses\n");
  console_color_reset();
  printf("\n\n");
}