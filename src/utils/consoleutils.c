#include <stdint.h>
#include <stdio.h>

/*
Macros for setting console position
*/
#define GOTOXY(x, y) printf("%c[%d;%df", 0x1B, y, x);
#define RESETXY printf("\033[H")

void console_gotoxy(uint16_t x, uint16_t y)
{
  GOTOXY(x, y);
}

void console_pos_reset()
{
  RESETXY;
}

/*
Macros for setting foreground and background colors based on ANSI
*/
#define ANSI_SET_FG_COLOR(x) printf("\033[38;5;%dm", x)
#define ANSI_SET_BG_COLOR(x) printf("\033[48;5;%dm", x)

#define ANSI_SET_DEFAULT_FG_COLOR printf("\033[39m")
#define ANSI_SET_DEFAULT_BG_COLOR printf("\033[49m")

/*
Multiple functions to control colors in general :PPP
*/

void console_foreground_set(uint8_t color)
{
  ANSI_SET_FG_COLOR(color);
}

void console_background_set(uint8_t color)
{
  ANSI_SET_BG_COLOR(color);
}

void console_foreground_reset()
{
  ANSI_SET_DEFAULT_FG_COLOR;
}

void console_background_reset()
{
  ANSI_SET_DEFAULT_BG_COLOR;
}

void console_color_reset()
{
  ANSI_SET_DEFAULT_FG_COLOR;
  ANSI_SET_DEFAULT_BG_COLOR;
}

/* Platform Specific behaviour */
#ifdef _WIN32
#include <windows.h>
#include <stdlib.h>

#define psleep(x) Sleep(x)
#define clrscr system("cls")

#elif defined(__unix__) || defined(__APPLE__) || defined(__linux__)

#include <unistd.h>

#define psleep(x) usleep(x * 1000000)
#define clrscr printf("\033[2J\033[1;1H")

#else
#error This target cannot be compiled. Please add definitions for your current build system.
#endif

void clear_screen()
{
  clrscr;
}

void csleep(double seconds)
{
  psleep(seconds);
}