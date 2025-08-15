/* Create portable getch() function */

// THANKS TO niko & S.S. Anne for the code
// https://stackoverflow.com/questions/7469139/what-is-the-equivalent-to-getch-getche-in-linux

#ifdef _WIN32
#include <conio.h>

#elif defined(__unix__) || defined(__APPLE__) || defined(__linux__)
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

static struct termios oldt, newt;

void init_term(void)
{
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void reset_term(void)
{
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

int kbhit(void)
{
  struct timeval tv = {0L, 0L};
  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);
  return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
}

int getch(void)
{
  unsigned char c;
  if (read(STDIN_FILENO, &c, 1) < 0)
    return -1;
  return c;
}

#else
#error This target cannot be compiled. Please add definitions for your current build system.
#endif

#include <stdio.h>
#include <limits.h>
#include <stdint.h>

#include "input.h"

vkey_t get_key(void)
{
#ifdef _WIN32
  if (_kbhit())
  {
    int ch = _getch();

    /* ESC */
    if (ch == 27)
      return VK_ESCAPE;

    /* Special keys (arrows, function keys, etc.) */
    if (ch == 0 || ch == 224)
    {
      ch = _getch();
      switch (ch)
      {
      case 72:
        return VK_UP;
      case 80:
        return VK_DOWN;
      case 75:
        return VK_LEFT;
      case 77:
        return VK_RIGHT;
      default:
        return VK_NONE;
      }
    }

    if (ch == '\r')
      return VK_ENTER;
    /* Normal keys: return ord value directly */
    return (vkey_t)(unsigned char)ch;
  }
#else
  if (kbhit())
  {
    int ch = getch();

    /* ESC or Escape sequences */
    if (ch == 27)
    {
      if (!kbhit())
        return VK_ESCAPE; // Single ESC press
      if (getch() == '[')
      {
        switch (getch())
        {
        case 'A':
          return VK_UP;
        case 'B':
          return VK_DOWN;
        case 'C':
          return VK_RIGHT;
        case 'D':
          return VK_LEFT;
        }
      }
      return VK_NONE;
    }

    return (vkey_t)(unsigned char)ch;
  }
#endif
  return VK_NONE;
}

#define INPUT_ERROR_MSG "Input error!\n"
#define INT_ERROR_MSG "Invalid number, try again.\n"

/* Reads an integer, returns INT32_MIN (<limits.h>) if there was an input error */
int32_t read_int(const char *prompt)
{
#ifndef _WIN32
  // Switch to canonical mode if not already
  reset_term();
#endif
  char line[100];
  int value;

  while (1)
  {
    printf("%s", prompt);
    if (!fgets(line, sizeof(line), stdin))
    {
      printf(INPUT_ERROR_MSG);
#ifndef _WIN32
      /* Activate terminal mode again */
      init_term();
#endif
      return INT32_MIN;
    }

    if (sscanf(line, "%d", &value) == 1)
    {
#ifndef _WIN32
      /* Activate terminal mode again */
      init_term();
#endif
      return value;
    }

    printf(INT_ERROR_MSG);
  }
#ifndef _WIN32
  /* Activate terminal mode again */
  init_term();
#endif
}
