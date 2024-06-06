#include "game.h"

int selected = -1;

void update_loop(ALLEGRO_EVENT* event, ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font)
{
    // update loop (when timer ticks)
    if (event->type == ALLEGRO_EVENT_TIMER) {
        draw_vertices(display, font, selected);
    }

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
    else if (event->type == ALLEGRO_EVENT_MOUSE_AXES) {
        mouse_x = event->mouse.x;
        mouse_y = event->mouse.y;

        float norm_mx = mouse_x - grid_offset.x;
        float norm_my = mouse_y - grid_offset.y;

        int row = floor((norm_my + MAP_CELL_SIZE * 0.25) / (MAP_CELL_SIZE * 0.75));
        int col = floor((norm_mx + MAP_CELL_SIZE * 0.25) / (MAP_CELL_SIZE * 0.5));
        int idx = row * (MAP_COLS * 2 + 1) + col;
        
        PLACEMENT v = board.placements[idx];
        if (v.active == false) return;
        float d = sqrt((v.x - norm_mx) * (v.x - norm_mx) + (v.y - norm_my) * (v.y - norm_my));
        if (d > MAP_CELL_SIZE * 0.1) { selected = -1; return; }
        
        selected = idx;
    }
}
