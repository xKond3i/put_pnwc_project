#include "game.h"

void update_loop()
{
    // clear screen
    // al_clear_to_color(al_map_rgb(96, 76, 200));

    // show what's been done in that frame
    al_flip_display();
}

void handle_events(ALLEGRO_EVENT* event, bool* running)
{
    if (event->type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        *running = false;
    }
    else if (event->type == ALLEGRO_EVENT_KEY_UP) {
        if (event->keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            *running = false;
        }
    }
}
