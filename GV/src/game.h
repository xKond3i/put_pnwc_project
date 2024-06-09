#pragma once

#include <stdio.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>

#include "map.h"
#include "ui.h"

float mouse_x;
float mouse_y;

int map_update_counter;

extern int selected;

// bank info
extern const int MAX_TYPE_CARDS;
extern int bank_cards[5];

// player info
extern ALLEGRO_COLOR PlayerColors[4];
extern const int MAX_PLAYERS;
extern int player_cards[4][5];
extern int player_points[4];

// turn info
extern int current_player;
extern int turn_nr;
extern int dices[2];

// winning conditions
extern int goal;

// init, free and restart
bool init_game(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font);
void restart_game(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font);
void clear_game_mem();

// logic
void update_loop(ALLEGRO_EVENT* event, ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, ALLEGRO_BITMAP* trophy);
void handle_events(ALLEGRO_EVENT* event, bool* running);
void next_turn();
