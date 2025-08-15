#include <string.h>

#include "templates.h"

void template_init(Template *templ, const char *name, uint16_t width, uint16_t height, uint16_t bomb_amount)
{
  /* Copy the name to the struct */
  memcpy(templ->name, name, sizeof(templ->name) - 1);
  /* Null terminate it just in case */
  templ->name[sizeof(templ->name) - 1] = '\0';

  templ->width = width;
  templ->height = height;
  templ->bomb_amount = bomb_amount;
}

void template_colors(Template *templ, uint8_t fg_color, uint8_t bg_color)
{
  templ->fg_color = fg_color;
  templ->bg_color = bg_color;
}