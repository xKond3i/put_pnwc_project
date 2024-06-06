#pragma once

#include <stdio.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "map.h"

float mouse_x;
float mouse_y;
int selected;

void update_loop(ALLEGRO_EVENT* event, ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font);
void handle_events(ALLEGRO_EVENT* event, bool* running);
