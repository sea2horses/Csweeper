#ifndef CONSOLE_UTILS_H
#define CONSOLE_UTILS_H

#include <stdint.h>

void console_gotoxy(uint16_t x, uint16_t y);
void console_pos_reset();

/*
Editable enum with the most common colors assigned to their ANSI equivalents

More info here:
https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
*/
typedef enum
{
  CC_RED = 1,
  CC_GREEN = 2,
  CC_YELLOW = 3,
  CC_BLUE = 4,
  CC_MAGENTA = 5,
  CC_CYAN = 6,
  CC_LIGHT_GRAY = 7,
  CC_DARK_GRAY = 8,
  CC_WHITE = 15,
  CC_DARK_GREEN = 28,
} ConsoleColors;

/*
Functions for added type checking and just ease of use
*/
void console_foreground_set(uint8_t color);
void console_background_set(uint8_t color);
void console_foreground_reset();
void console_background_reset();
void console_color_reset();

void clear_screen();
void csleep(double seconds);

#endif /* CONSOLE_UTILS_H */