#include "game.h"

int selected = -1;

const int max_type_cards = 19;
int bank_cards[5];

int turn_nr;
int dices[2];

bool init_game(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font)
{
    if (!init_map(display)) return 0;

    restart_game(display, font);
}

void restart_game(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font)
{
    turn_nr = 0;

    for (int i = 0; i < RESOURCES_COUNT; ++i) { bank_cards[i] = max_type_cards; }

    draw_map(display, font, selected);
}

void update_loop(ALLEGRO_EVENT* event, ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font)
{
    // ui
    draw_bank_cards(display, font);

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

void draw_bank_cards(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font)
{
    ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
    ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);
    float padding = 16.f;
    float margin = 8.f;
    float line_height = al_get_font_line_height(font);
    float max_width = al_get_text_width(font, "BANK CARDS:");
    al_draw_filled_rounded_rectangle(margin, margin,
                                     margin + max_width + 2 * padding, (line_height + margin) * (RESOURCES_COUNT + 1) + padding * 3,
                                     padding, padding, white);
    float x = padding + margin;
    float y = padding + margin;
    al_draw_text(font, black, x, y, 0, "BANK CARDS:");
    y += padding + line_height;
    x += margin;
    for (int i = 0; i < RESOURCES_COUNT; ++i) {
        char name[32];
        char count[4];
        _itoa_s(bank_cards[i], count, 4, 10);
        //sprintf_s(&text, 32, "%s: %d", RESOURCES_NAMES[i], bank_cards[i]);
        sprintf_s(&name, 32, "%s:", RESOURCES_NAMES[i]);
        al_draw_filled_rounded_rectangle(margin + padding, y, max_width + margin + padding, y + line_height, margin, margin, ResourceColor[i + 2]);
        al_draw_text(font, white, x, y, 0, name);
        al_draw_text(font, white, x + max_width - 2 * margin - al_get_text_width(font, count), y, 0, count);
        y += margin + line_height;
    }
}
