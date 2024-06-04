#pragma once

#include <stdio.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

void update_loop();
void handle_events(ALLEGRO_EVENT* event, bool* running);
