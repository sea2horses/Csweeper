#ifndef GAME_H
#define GAME_H

#include <stdint.h>

#include "../classes/templates.h"

/**
 * start_custom_game
 * Start a custom game (no templates)
 * @param width The width of the game board
 * @param height The height of the game board
 * @param bomb_amount The number of bombs to place
 */
void start_custom_game(uint16_t width, uint16_t height, uint16_t bomb_amount);

/**
 * start_template_game
 * Start a templated game
 * @param templ Pointer to the template to use for the game
 */
void start_template_game(Template *templ);

#endif /* GAME_H */