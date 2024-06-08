#pragma once

#include <stdio.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "map.h"

float mouse_x;
float mouse_y;

extern int selected;

extern const int max_type_cards;
extern int bank_cards[5];

extern int turn_nr;
extern int dices[2];

bool init_game(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font);
void restart_game(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font);

void update_loop(ALLEGRO_EVENT* event, ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font);
void handle_events(ALLEGRO_EVENT* event, bool* running);
void draw_bank_cards(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font);
