#ifndef GETCH_H
#define GETCH_H

#include <stdint.h>

/* --- Key Enum --- */
typedef enum
{
  VK_NONE = 0,
  VK_ESCAPE = 256, // start special keys above normal char range
  VK_LEFT,
  VK_RIGHT,
  VK_UP,
  VK_DOWN,
  VK_ENTER = 10
} vkey_t;

#ifdef _WIN32
#include <conio.h>
#elif defined(__unix__) || defined(__APPLE__) || defined(__linux__)

void init_term(void);

void reset_term(void);

int kbhit(void);

int getch(void);

vkey_t get_key(void);

int32_t read_int(const char *prompt);

#endif /* UNIX */

#endif /* GETCH_H */
