#ifndef TEMPLATES_H
#define TEMPLATES_H

#include <stdbool.h>
#include <stdint.h>

/* Minefield struct definition */
typedef struct
{
  char name[30];
  uint16_t width;
  uint16_t height;
  uint16_t bomb_amount;
  uint8_t fg_color;
  uint8_t bg_color;
} Template;

void template_init(Template *templ, const char *name, uint16_t width, uint16_t height, uint16_t bomb_amount);
void template_colors(Template *templ, uint8_t fg_color, uint8_t bg_color);

#endif /* TEMPLATES_H */