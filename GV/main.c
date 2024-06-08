#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

// modules
#include "src/game.h"

int main()
{
    // init allegro
    al_init();

    // init variables
    ALLEGRO_DISPLAY* display;
    ALLEGRO_EVENT_QUEUE* queue;
    ALLEGRO_TIMER* timer;
    ALLEGRO_FONT* font;

    // assign variables
    display = al_create_display(1280, 720);
    queue = al_create_event_queue();
    timer = al_create_timer(1.0 / 30);

    // some setup
    al_set_window_title(display, "Grow a Village!");

    // init allegro modules
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_install_keyboard();
    al_install_mouse();

    // turn on blending for alpha colors
    //al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

    // register event sources
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_display_event_source(display));

    // load resources
    font = al_load_ttf_font("resources/firacode.ttf", 24, 0);

    // init game modules
    al_start_timer(timer);
    srand(time(NULL));
    if (!init_game(display, font)) return 0;

    // * MAIN GAME LOOP
    bool running = true;
    while (running) {
        // handle events
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);
        handle_events(&event, &running);
        update_loop(&event, display, font);
    }

    // clear pointers to free memory
    free_map();
    al_uninstall_mouse();
    al_uninstall_keyboard();
    al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);
    al_shutdown_ttf_addon();
    al_shutdown_font_addon();
    al_shutdown_primitives_addon();
    return 0;
}
