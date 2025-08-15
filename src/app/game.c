/**
 * game.c
 * Implementation of the Minesweeper game logic and rendering.
 *
 * This file contains the core logic for the Minesweeper game, including game initialization,
 * rendering, user input handling, and game state management. It provides functions to start
 * a custom or templated game, manage the game loop, handle user interactions, and render
 * the game board and GUI.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "../classes/minefield.h"
#include "../classes/vec.h"
#include "../utils/consoleutils.h"
#include "../utils/input.h"

/*
 * Little macro to clamp a value between 2 limits
 * used to clamp mouse positions to the according limits in the game
 */
#define clamp(low, hi, x) ((x > hi) ? hi : ((x < low) ? low : x))
/*
 * a for loop was too ugly
 */
#define repeat(x) for (int32_t _rrxvalno_ = 0; _rrxvalno_ < x; _rrxvalno_++)

/* For text drawing purposes */
typedef enum
{
  LEFT,
  RIGHT,
  CENTER
} TextAlign;

/* The color palette of all the numbers */
static uint8_t mine_colors[] = {
    CC_LIGHT_GRAY, CC_BLUE, CC_GREEN, CC_RED, CC_CYAN, CC_YELLOW, CC_MAGENTA, CC_LIGHT_GRAY, CC_DARK_GRAY};

/*
 * Ok, so let's actually explain everything going on here
 * start_game() is the final function that uses the attributes set by the header's functions
 * start_template_game() and start_custom_game()
 * this function does NOT set the current_template, width, height and bomb_amount attributes
 *
 * What this function is responsible of is allocating the game board (with _allocate_board),
 * call the generate_bombs and generate_blessing functions to ready up everything and finally
 * entering game_loop
 *
 * When game_loop stops, this function is the responsible of deallocating the board.
 */
static void start_game();

/*
 * game_loop() contains all the game logic that runs consistently: inputs, cursor movement
 * and calls the appropriate functions to actually run the game, also uses the _draw functions
 * to update whatever's going on in the board.
 *
 * It is basically the responsible of handling everything after the game has started,
 * when do_game_loop is set to false, the game loop breaks and the board is deallocated through start_game
 */
static void game_loop();

/*
 * Both of these functions are really similar, just hardcoded win and lose animations here
 * EVERYTHING is in these functions, so if you want to edit the length of the wait after winning/losing
 * the colors, the messages, these functions are your friends, they themselves set do_game_loop to false.
 * They're usually called from game_loop() and so you can just check the code in there to see how they're called.
 *
 * One last thing, _game_over_animation can ALSO be triggered through _mine_field, which is something I didn't necessarily
 * want to do, but when you trigger a false sweep (the flag amount a free space is accurate to the mine count but their positions aren't)
 * _game_over_animation is also called, you can go check in _mine_field. In any other case, it's impossible to trigger it from there
 * since do_game_loop doesn't call _mine_field when you try to show a field with a mine on it.
 */
static void _game_over_animation();
static void _win_animation();

/**
 * For sweeping a field, a.k.a you already placed enough flags around it to just show the rest of
 * spaces.
 *
 * Imagine a situation like this:
 * [ ][ ][ ]
 * [ ][1][F]
 * [ ][ ][ ]
 *
 * You already know that the mine is on the 'F' square, but you now need to painstakingly reveal the
 * rest of spaces one by one, with this function, these cases are detected and triggered when
 * pressing enter (a.k.a mining) on an already shown SAFE space, this way, revealing every other square
 * at once, which greatly improves the player's speed and experience.
 *
 * Note: You can also do what's known as 'false sweeping', when the case previously known is TRUE,
 * meaning that the amount of flags around the square corresponds to the bomb amount around the field,
 * but that guess simply being wrong, in this case, the spot with the bomb will also be revealed and such
 * losing you the game, this will trigger a _game_over_animation as said in that functions's section.
 * @param x The x coordinate of the field
 * @param y The y coordinate of the field
 */
static void _sweep_field(uint16_t x, uint16_t y);

/**
 * Mine function for a field. Pretty simple. Shows or 'mines' a field
 *
 * Contains a fail safe to not act on already-shown spaces, thus preventing
 * an infinite recursion bug paired with _show_surrounding_fields.
 *
 * If the current space's bomb amount is 0, it will recursively use _show_surrounding_fields
 * to create the 'island' effect on the original minesweeper.
 * @param x The x coordinate of the field
 * @param y The y coordinate of the field
 */
static void _show_field(uint16_t x, uint16_t y);

/**
 * Shows all surrounding fields around the field on (x, y), pretty simple too.
 *
 * The 'bypass_flags' argument exists due to weird interactions when working with both sweeping and showing:
 *
 * Let's think about this position:
 *
 * [F][F][ ]
 * [2][2][1]
 * [0][1][ ]
 *
 * When sweeping the center 2, the top-right and bottom-right spaces need to be shown,
 * the ones without flags, but the already flagged spaces (which contain bombs) must not be shown.
 * This means that we don't ignore the flags, then: bypass_flags = false.
 *
 * But why don't we just not show flagged fields at all then?? Well, let's think:
 * [ ][F][F]
 * [ ][2][ ]
 * [ ][1][1]
 *
 * This is just a silly position where the user placed two flags randomly around the 2, but
 * let's imagine that the middle-right square is actually a 0:
 * [ ][F][F]
 * [ ][2][0]
 * [ ][1][1]
 *
 * Since the expected behaviour when mining a 0 is to show ALL surrounding fields (they don't have bombs anyway)
 * if we don't ignore the flags, we'll get a weird situation, where you have to un-flag the fields to actually
 * reveal them, instead of the seamless way 0s normally work, so, in this case, we actually WANT to ignore
 * the flags, threfore bypass_flags = true.
 *
 * @param x The x coordinate of the center field
 * @param y The y coordinate of the center field
 * @param bypass_flags Whether to show flagged fields anyway
 */
static void _show_surrounding_fields(uint16_t x, uint16_t y, bool bypass_flags);

/**
 * Draws the cell at the (x, y) position on the board, used to draw the whole board and to update
 * fields when stuff happens to them (being shown, cursor hovering over them)
 *
 * Highlight is a color you can use to simulate the cursor or simply any highlight you want,
 * it will ONLY affect the brackets '[ ]' and by default will be set to the background, while
 * the text itself will always be white
 *
 * also, this method is NOT used to draw the game over and winning animations,
 * due to them requiring to paint the whole cell in a different way, they do it
 * differently.
 *
 * Final note: The reason why you also need to pass the board pointer is a weird artifact
 * dating to the first time I implemented this. Since I did it so without using any static variables
 * I had to pass EVERYTHING EVERYTIME, thankfully I eventually decided to just have the necessary game info
 * available to all functions but this fucker still remains, alongside other functions
 *
 * @param board The game board
 * @param x The x coordinate of the cell
 * @param y The y coordinate of the cell
 * @param highlight Highlight color (0 for none)
 */
static void _draw_cell(Minefield **board, uint16_t x, uint16_t y, uint8_t highlight);

/*
 * Draws the board using the variables defined in Game Info
 */
static void _draw_board();

/*
 * Draws the game GUI, the part below the board hehehhehehehehheheh
 */
static void _draw_game_gui();

/**
 * Text drawing utility,
 * will draw at position (x,y) on the console a given string (text)
 * aligned in some way,
 *
 * If you want your text going left to right, use LEFT (used for the mines placed and regularly the one you want)
 * right to left, use RIGHT (used for the seconds on-screen)
 * aligned at the center, use CENTER (used for the template name)
 *
 * @param text The string to draw
 * @param x The x position on the console
 * @param y The y position on the console
 * @param alignment The text alignment (LEFT, RIGHT, CENTER)
 */
static void _draw_text(const char *text, uint16_t x, uint16_t y, TextAlign alignment);

/**
 * Creates a dynamically allocated 2D array of the Minefield type,
 * returns NULL if allocation was unsuccesful, this is used and checked in
 * start_game which also sets it to the 'game_board' variable.
 *
 * This function only CREATES the array, it doesn't set it to anything by default
 * this behaviour is done in start_game
 *
 * @param width The width of the board
 * @param height The height of the board
 * @return Pointer to the allocated board, or NULL on failure
 */
static Minefield **_allocate_board(uint16_t width, uint16_t height);

/**
 * Sets up bomb amounts and bombs in the given board 2D pointer
 *
 * Basically just generates an array with field IDs and then shuffles it,
 * the first `bomb_amount` fields on the array will be the elected bombs,
 * then you just turn up the bomb_amount on all their neighbours.
 *
 * @param board The game board
 * @param width The width of the board
 * @param height The height of the board
 * @param bomb_amount The number of bombs to place
 */
static void _generate_bombs(Minefield **board, uint16_t width, uint16_t height, uint16_t bomb_amount);
/*
 * Fills the noguess_blessing variable, you can read its documentation
 * for better info about what's going on.
 */
static void _generate_blessing();

/* All Game info */

/* Dynamically allocated game board 2D array */
static Minefield **game_board = NULL;
/* Current template pointer (set by start_template_game), ALWAYS check if NULL */
static Template *current_template = NULL;
/* Game dimensions */
static uint16_t game_width = 0;
static uint16_t game_height = 0;
static uint16_t game_bomb_amount = 0;
/* Cursor position used in _game_loop and a lot other functions */
static Vec2 cursor_position;
/* Starting time for the game, useful for calculating seconds_passed */
static time_t start_timestamp = 0;
/* Time you see on-screen when playing */
static uint16_t seconds_passed = 0;
/*
 * Var set in _show_field useful to detect the win condition
 * (correct_guesses == game_width * game_height - bomb_amount)
 * which means that every single field that wasn't a bomb was discovered
 *
 * This exact code sample exists in _game_loop
 */
static uint16_t correct_guesses = 0;
/* Control variable useful to the _draw_gui function */
static uint16_t flags_placed = 0;
/*
 * Although the game is not TRULY a no guessing mode
 * (erradicating the 50/50s and guesses in general would require for me to build a solver)
 * the No Guess Blessing is a vector that contains one randomly chosen 0 space.
 *
 * This way, you can get a guaranteed* island at the start of the game.
 * Your cursor will also be placed automatically on the blessing at the beginning of the game.
 *
 * asterisk: The blessing is not guaranteed, if there are no 0 spaces when first building the board
 * the blessing will stay at -1, -1 and nothing will be highlighted at the start.
 *
 * The cursor will be placed at 0, 0 if this does happen.
 */
static Vec2 noguess_blessing = {.x = -1, .y = -1};
/* Variable to control the game flow (turn false to stop the game) */
static bool do_game_loop = true;

/* EXCESSIVE COMMENTING ENDS NOW! most of the code should be really clear */

/* Separate functions to prevent redundant arguments */

/* Start a custom game (no templates) */
void start_custom_game(uint16_t width, uint16_t height, uint16_t bomb_amount)
{
  /* Set the game variables */
  game_width = width;
  game_height = height;
  game_bomb_amount = bomb_amount;
  /* Set no template */
  current_template = NULL;

  start_game();
}

/* Start a templated game */
void start_template_game(Template *templ)
{
  /* Set the game variables */
  game_width = templ->width;
  game_height = templ->height;
  game_bomb_amount = templ->bomb_amount;
  /* Set the template (for drawing purposes) */
  current_template = templ;

  start_game();
}

/* Start a game with the set game variables */
static void start_game()
{
  clear_screen();
  console_color_reset();

  /* Introduce randomness */
  srand(time(0));

  /* Let's create the board */
  game_board = _allocate_board(game_width, game_height);
  if (game_board == NULL)
  {
    printf("There was a problem generating the game, returning to the main menu...");
    csleep(2);
    return;
  }

  /* If we reach this point, all memory was succesfully allocated */
  _generate_bombs(game_board, game_width, game_height, game_bomb_amount);
  _generate_blessing(); /* No guess mode */

  game_loop();

  /* Let's free all the memory */
  for (uint16_t i = 0; i < game_height; i++)
    free(game_board[i]);
  free(game_board);
}

// static void game_draw(const uint16_t width, const uint16_t height)
// {
// }

static void game_loop()
{
  /* If blessing exists, set current position to it, else, to 0 0 */
  Vec2 invalid_blessing = {.x = -1, .y = -1};

  if (!vec_cmpr(noguess_blessing, invalid_blessing))
  {
    cursor_position.x = noguess_blessing.x;
    cursor_position.y = noguess_blessing.y;
  }
  else
  {
    cursor_position.x = 0;
    cursor_position.y = 0;
  }

  /* Timer purposes */
  start_timestamp = time(0);

  /* Cooldowns */
  const uint8_t key_cooldown = 15; /* This is useless due to the way im gauging input but whatever */
  int32_t last_time_update = -1;
  uint8_t current_key_cooldown = 0;
  uint8_t enter_grace_period = 0; /* This is unused btw, chat should I delete it??? */

  /* Assure some variables are in their correct starting values */
  correct_guesses = 0;
  do_game_loop = true;
  flags_placed = 0;

  _draw_board();
  /* Draw the initial position of the cursor */
  _draw_cell(game_board, cursor_position.x, cursor_position.y, CC_DARK_GREEN);

  /* Game Loop */
  while (do_game_loop)
  {
    seconds_passed = difftime(time(0), start_timestamp);
    seconds_passed = (seconds_passed > 9999) ? 9999 : seconds_passed;

    Vec2 old_cursor_position = cursor_position;

    /* Get key presses */
    vkey_t key = get_key();

    if (key == VK_ESCAPE)
      break;

    /* Refresh display */
    if (key == 'r' || key == 'R')
    {
      /* Reset the screen */
      clear_screen();
      _draw_board();
      /* Draw the initial position of the cursor */
      _draw_cell(game_board, cursor_position.x, cursor_position.y, CC_DARK_GREEN);
      /* Draw GUI */
      _draw_game_gui();
    }

    /* Flag a field */
    if (key == 'f' || key == 'F')
    {
      Minefield *field = &game_board[cursor_position.y][cursor_position.x];
      /* Only if it hasn't been shown yet */
      if (!field->is_mined)
      {
        /* Toggle flag */
        field->is_flagged = !field->is_flagged;
        /* This little trick will take 1 from flags if it's a 0, and add 1 if it's a 1*/
        flags_placed += (int8_t)((field->is_flagged - 0.5) * 2.0);
      }

      /* Let's go update it */
      _draw_cell(game_board, cursor_position.x, cursor_position.y, CC_DARK_GREEN);
      /* And update the GUI */
      _draw_game_gui();
    }

    /* Movement Logic */
    if (!current_key_cooldown && key != VK_NONE)
    {
      /* Assume there was a key pressed */
      bool key_pressed = true;

      /* Move mouse according to the key pressed */
      switch (key)
      {
      case (VK_LEFT):
        cursor_position.x--;
        break;

      case (VK_RIGHT):
        cursor_position.x++;
        break;

      case (VK_UP):
        cursor_position.y--;
        break;

      case (VK_DOWN):
        cursor_position.y++;
        break;

      /* If there was no key pressed, set the variable to false */
      default:
        key_pressed = false;
        break;
      }

      /* If there was a key press, set the cooldown */
      if (key_pressed)
        current_key_cooldown = key_cooldown;

      /* Clamp the values to the appropriate limits */
      cursor_position.x = clamp(0, game_width - 1, cursor_position.x);
      cursor_position.y = clamp(0, game_height - 1, cursor_position.y);
    }

    if (current_key_cooldown != 0)
      current_key_cooldown--;

    if (key == VK_ENTER)
    {
      Minefield *field = &game_board[cursor_position.y][cursor_position.x];

      /* Depending of the state of the mine, we do certain actions */
      if (field->has_bomb)
        _game_over_animation();
      else if (field->is_mined)
        _sweep_field(cursor_position.x, cursor_position.y);
      else if (!field->is_flagged)
        _show_field(cursor_position.x, cursor_position.y);

      /* Win condition */
      if (correct_guesses == (game_width * game_height) - game_bomb_amount)
        _win_animation();

      enter_grace_period = 120; /* Useless bitch */
    }

    if (!vec_cmpr(cursor_position, old_cursor_position) || enter_grace_period > 0)
    {
      if (enter_grace_period > 0)
        enter_grace_period--;
      _draw_cell(game_board, old_cursor_position.x, old_cursor_position.y, false);
      _draw_cell(game_board, cursor_position.x, cursor_position.y, (enter_grace_period > 0) ? CC_RED : CC_DARK_GREEN);
    }

    /* Only redraw GUI if time is outdated */
    if (last_time_update != seconds_passed)
    {
      last_time_update = seconds_passed;
      _draw_game_gui();
    }

    /* Flush the standard output to get everything drawn instantly thrown on screen */
    console_gotoxy(1, game_height + 3);
    fflush(stdout);
  }
}

static void _game_over_animation()
{
  for (uint16_t i = 0; i < game_height; i++)
    for (uint16_t j = 0; j < game_width; j++)
    {
      if (!game_board[i][j].has_bomb)
        continue;

      console_gotoxy(j * 3 + 1, i + 1);

      console_foreground_set(CC_WHITE);
      console_background_set(CC_RED);

      printf("[X]");
    }

  console_color_reset();

  console_foreground_set(CC_YELLOW);
  _draw_text("Better luck next time!", (game_width * 3) / 2.0 + 1, game_height + 3, CENTER);
  console_color_reset();

  fflush(stdout);
  csleep(4);
  do_game_loop = false;
}

static void _win_animation()
{
  flags_placed = game_bomb_amount;
  _draw_game_gui();

  console_foreground_set(CC_BLUE);
  _draw_text("YOU WON!", (game_width * 3) / 2.0 + 1, game_height + 3, CENTER);

  console_foreground_set(CC_GREEN);
  _draw_text("good job!", (game_width * 3) / 2.0 + 1, game_height + 4, CENTER);

  uint16_t iterations = 0;
  repeat(10)
  {
    for (uint16_t i = 0; i < game_height; i++)
      for (uint16_t j = 0; j < game_width; j++)
      {
        if (!game_board[i][j].has_bomb)
          continue;

        console_gotoxy(j * 3 + 1, i + 1);

        // Color changing animation
        if (iterations % 2 == 0)
        {
          console_foreground_set(CC_BLUE);
          console_background_set(CC_YELLOW);
        }
        else
        {
          console_foreground_set(CC_YELLOW);
          console_background_set(CC_BLUE);
        }

        printf("[!]");
      }

    fflush(stdout);
    csleep(0.5);

    iterations++;
  }

  console_color_reset();
  csleep(2);

  do_game_loop = false;
}

/* 'Sweep' a mine field (equivalent to shift-click on og minesweeper) */
static void _sweep_field(uint16_t x, uint16_t y)
{
  if (!game_board[y][x].is_mined)
    return;

  uint8_t flag_count = 0;

  for (int32_t i = -1; i <= 1; i++)
  {
    /* Limit detection */
    if (y + i < 0 || y + i >= game_height)
      continue;
    for (int32_t j = -1; j <= 1; j++)
    {
      /* Limit detection */
      if (x + j < 0 || x + j >= game_width)
        continue;
      flag_count += game_board[y + i][x + j].is_flagged;
      ;
    }
  }

  if (flag_count == game_board[y][x].bomb_amount)
    _show_surrounding_fields(x, y, false);
}

static void _show_field(uint16_t x, uint16_t y)
{
  if (game_board[y][x].is_mined)
    return;

  game_board[y][x].is_mined = true;
  game_board[y][x].is_flagged = false;
  /* Update the cell */
  _draw_cell(game_board, x, y, false);
  if (!game_board[y][x].has_bomb)
    correct_guesses++;
  else
    /* You lost */
    _game_over_animation();

  /* If the field has no bombs surrounding it, show all surrounding fields */
  if (game_board[y][x].bomb_amount == 0)
    _show_surrounding_fields(x, y, true);
}

static void _show_surrounding_fields(uint16_t x, uint16_t y, bool bypass_flags)
{
  for (int32_t i = -1; i <= 1; i++)
  {
    /* Limit detection */
    if (y + i < 0 || y + i >= game_height)
      continue;
    for (int32_t j = -1; j <= 1; j++)
    {
      /* Limit detection */
      if (x + j < 0 || x + j >= game_width)
        continue;

      /* ONLY surrounding fields */
      if (i == 0 && j == 0)
        continue;

      /* If bypass_flags is off */
      if (!bypass_flags && game_board[y + i][x + j].is_flagged)
        continue;

      _show_field(x + j, y + i);
    }
  }
}

static void _draw_cell(Minefield **board, uint16_t x, uint16_t y, uint8_t highlight)
{
  console_gotoxy(x * 3 + 1, y + 1);
  if (highlight)
  {
    console_background_set(highlight);
    console_foreground_set(CC_WHITE);
    printf("[ ]");
    console_color_reset();
  }
  else
  {
    console_color_reset();
    printf("[ ]");
  }

  console_gotoxy(x * 3 + 2, y + 1);

  // Example: show covered cell, revealed cell, or flagged cell
  if (board[y][x].is_mined)
  {
    if (board[y][x].has_bomb)
    {
      printf("X");
    }
    else
    {
      console_foreground_set(mine_colors[board[y][x].bomb_amount]);
      printf("%d", board[y][x].bomb_amount);
    }
  }
  else if (board[y][x].is_flagged)
  {

    console_background_set(CC_RED);
    console_foreground_set(CC_WHITE);
    printf("F");
  }
  else
  {
    Vec2 pos_as_vec = {.x = x, .y = y};
    if (vec_cmpr(noguess_blessing, pos_as_vec))
    {
      console_foreground_set(CC_GREEN);
      printf("X");
    }
    else
      printf(" ");
  }
  console_color_reset();
}

static void _draw_board()
{
  /* Go to the start */
  console_pos_reset();
  for (uint16_t i = 0; i < game_height; i++)
  {
    for (uint16_t j = 0; j < game_width; j++)
    {
      _draw_cell(game_board, j, i, false);
    }
    printf("\n");
  }
}

static void _draw_game_gui()
{
  /* Clean the old GUI up */
  console_gotoxy(1, game_height + 1);
  repeat(2)
  {
    /* Basically print enough spaces to get rid of anything drawn before */
    char override[game_width * 3 + 1];
    memset(override, ' ', game_width * 3);
    override[game_width * 3] = '\0';

    printf("%s\n", override);
  }

  console_color_reset();

  /* Draw the GUI */
  char flags_strings[20];
  sprintf(flags_strings, "%d/%d mines", flags_placed, game_bomb_amount);
  _draw_text(flags_strings, 1, game_height + 1, RIGHT);

  /* Draw the seconds passed */
  char time_string[10];
  sprintf(time_string, "%04d", seconds_passed);
  _draw_text(time_string, game_width * 3 + 1, game_height + 1, LEFT);

  /* Draw the template */
  if (current_template != NULL)
  {
    console_foreground_set(current_template->fg_color);
    console_background_set(current_template->bg_color);
  }
  _draw_text((current_template == NULL) ? "Custom" : current_template->name, (game_width * 3) / 2.0 + 1, game_height + 2, CENTER);
  console_color_reset();
}

static void _draw_text(const char *text, uint16_t x, uint16_t y, TextAlign alignment)
{
  int32_t starting_x;

  /* Get the starting x */
  switch (alignment)
  {
  case (RIGHT):
    starting_x = x;
    break;

  case (LEFT):
    starting_x = x - strlen(text);
    break;

  case (CENTER):
    starting_x = (x - strlen(text) / 2.0) + 0.5;
    break;
  }

  if (starting_x <= 0)
    return;

  console_gotoxy(starting_x, y);
  printf("%s", text);
}

/* Function to allocate all the memory for the game and initialize all fields */
static Minefield **_allocate_board(uint16_t width, uint16_t height)
{
  Minefield **board = malloc(sizeof(Minefield *) * height);
  if (board == NULL)
  {
    return NULL;
  }

  for (uint16_t i = 0; i < height; i++)
  {
    board[i] = malloc(sizeof(Minefield) * width);

    if (board[i] == NULL)
    {
      /* Free all the memory that was succesfully allocated */
      for (uint16_t j = i - 1; j >= 0; j++)
        free(board[j]);
      free(board);
      return NULL;
    }
    else
      /* Init every space in the row */
      for (uint16_t j = 0; j < width; j++)
        init_minefield(&board[i][j]);
  }

  return board;
}

/* Functions related to in-game stuff */
static void _generate_bombs(Minefield **board, uint16_t width, uint16_t height, uint16_t bomb_amount)
{
  uint32_t arr[width * height];

  /* Create index of all bombs */
  for (uint32_t i = 0; i < width * height; i++)
    arr[i] = i;

  /* Shuffle the array */
  for (uint32_t i = 0; i < width * height - 1; i++)
  {
    uint32_t j = rand() % (width * height);
    int t = arr[j];
    arr[j] = arr[i];
    arr[i] = t;
  }

  /* Now convert the indexes generated to actual coordinates and populate the minefield */
  for (uint16_t i = 0; i < bomb_amount; i++)
  {
    uint16_t index = arr[i];
    uint16_t y = index / width;
    uint16_t x = index % width;
    board[y][x].has_bomb = 1;

    /* Up neighbours's bomb amount */
    for (int32_t k = -1; k <= 1; k++)
    {
      /* Limit detection */
      if (y + k < 0 || y + k >= height)
        continue;
      for (int32_t l = -1; l <= 1; l++)
      {
        if (x + l < 0 || x + l >= width)
          continue;

        board[y + k][x + l].bomb_amount++;
      }
    }
  }
}

static void _generate_blessing()
{
  /* Let's get the eligible spaces */
  Vec2 eligibles[game_width * game_height];
  uint16_t eligible_count = 0;

  for (uint16_t i = 0; i < game_height; i++)
    for (uint16_t j = 0; j < game_width; j++)
      /* If it has no stuff around it, make it eligible */
      if (game_board[i][j].bomb_amount == 0)
      {
        Vec2 eligible_coords = {.x = j, .y = i};
        /* Set the coordinates */
        eligibles[eligible_count] = eligible_coords;
        eligible_count++;
      }

  /* Shuffle the array */
  for (uint32_t i = 0; i < eligible_count; i++)
  {
    uint32_t j = rand() % eligible_count;
    Vec2 t = eligibles[j];
    eligibles[j] = eligibles[i];
    eligibles[i] = t;
  }

  /* First element is our blessing */
  if (eligible_count > 0)
    noguess_blessing = eligibles[0];
}