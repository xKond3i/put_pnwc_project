#pragma once

#include <stdio.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "map.h"

float mouse_x;
float mouse_y;

extern int selected;

extern const int MAX_TYPE_CARDS;
extern int bank_cards[5];

extern ALLEGRO_COLOR playerColors[4];
extern const int MAX_PLAYERS;
extern int current_player;
extern int turn_nr;
extern int dices[2];

// init, free and restart
bool init_game(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font);
void restart_game(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font);
void clear_game_mem();

// logic
void update_loop(ALLEGRO_EVENT* event, ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font);
void handle_events(ALLEGRO_EVENT* event, bool* running);
void next_turn();

// drawing
void draw_dice(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font);
void draw_turn_info(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font);
void draw_bank_cards(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font);
