#include <stdio.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

int main()
{
    // init allegro
    al_init();

    // init variables
    ALLEGRO_DISPLAY* display;
    ALLEGRO_EVENT_QUEUE* queue;
    ALLEGRO_TIMER* timer;

    // assign variables
    display = al_create_display(1280, 720);
    queue = al_create_event_queue();
    timer = al_create_timer(1.0 / 60);

    // init allegro modules
    al_set_window_title(display, "Grow a Village!");
    al_install_keyboard();
    al_install_mouse();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_display_event_source(display));
    al_start_timer(timer);

    // * MAIN GAME LOOP
    bool running = true;
    while (running) {
        // handle events
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }
        else if (event.type == ALLEGRO_EVENT_KEY_UP) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                running = false;
            }
        }
        // * UPDATE LOOP
        else if (event.type == ALLEGRO_EVENT_TIMER) {
            // clear screen
            al_clear_to_color(al_map_rgb(96, 76, 200));

            // show what's been done in that frame
            al_flip_display();
        }
    }

    // clear pointers to free memory
    al_uninstall_mouse();
    al_uninstall_keyboard();
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);
    return 0;
}
