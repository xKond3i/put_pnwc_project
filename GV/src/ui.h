#pragma once

#include <stdio.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>

#include "map.h"

// drawing
void draw_dice(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, float x, float y, int size, int number);
void draw_turn_info(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, ALLEGRO_BITMAP* trophy, int* dices, int turn_nr, int current_player, int* player_points, int goal, const int MAX_PLAYERS, ALLEGRO_COLOR* PlayerColors);
void draw_cards(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, int shift, const int* cards, char* title, ALLEGRO_COLOR color);
void draw_bank_cards(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, int* bank_cards);
void draw_player_cards(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, int* player_cards, ALLEGRO_COLOR player_color);
