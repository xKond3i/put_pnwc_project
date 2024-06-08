#include "game.h"

int selected = -1;

const int MAX_TYPE_CARDS = 19;
int bank_cards[5];

ALLEGRO_COLOR PlayerColors[4];
const int MAX_PLAYERS = 4;
int player_cards[4][5];
int player_points[4];
int current_player;
int turn_nr;
int dices[2];

bool init_game(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font)
{
    if (!init_map(display)) return 0;

    PlayerColors[0] = al_map_rgb(196, 0, 0);
    PlayerColors[1] = al_map_rgb(0, 0, 196);
    PlayerColors[2] = al_map_rgb(0, 128, 0);
    PlayerColors[3] = al_map_rgb(128, 0, 196);

    restart_game(display, font);
}

void clear_game_mem() {
    free_map();
}

void restart_game(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font)
{
    // reset bank stats
    for (int i = 0; i < RESOURCES_COUNT; ++i)
        bank_cards[i] = MAX_TYPE_CARDS;

    // reset player stats
    for (int p = 0; p < MAX_PLAYERS; ++p) {
        player_points[p] = 0;
        for (int i = 0; i < RESOURCES_COUNT; ++i)
            player_cards[p][i] = 0;
    }

    draw_map(display, font, selected);

    current_player = rand() % MAX_PLAYERS;
    turn_nr = 0;
    next_turn();
}

// LOGIC
void update_loop(ALLEGRO_EVENT* event, ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font)
{
    // update loop (when timer ticks)
    if (event->type == ALLEGRO_EVENT_TIMER) {
        if (map_update_counter == 0) draw_map(display, font, turn_nr <= MAX_PLAYERS ? -1 : dices[0] + dices[1]);
        map_update_counter = (map_update_counter + 1) % 30;

        draw_vertices(display, font, selected, PlayerColors[current_player]);
    }

    // ui
    draw_turn_info(display, font);

    //if (turn_nr <= MAX_PLAYERS) {
    //    float margin = 16.f;
    //    char text[] = "* placement choosing phase *";
    //    al_draw_filled_rectangle(0, 0, margin + al_get_text_width(font, text), margin + al_get_font_line_height(font), ResourceColor[WATER]);
    //    al_draw_text(font, al_map_rgb(255, 0, 0), margin, margin, 0, text);
    //}
    //else {
        draw_bank_cards(display, font);
        draw_player_cards(display, font);
    //}

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
        else if (event->keyboard.keycode == ALLEGRO_KEY_ENTER) {
            next_turn();
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

void next_turn() {
    ++turn_nr;
    current_player = (current_player + 1) % MAX_PLAYERS;
    if (turn_nr == 1) {
        dices[0] = current_player + 1;
        dices[1] = -1;
    }
    else {
        dices[0] = rand() % 6 + 1;
        dices[1] = rand() % 6 + 1;
    }
    map_update_counter = 0;
}

// DRAWING
void draw_dice(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, float x, float y, int size, int number) {
    ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
    ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);

    float r = size / 10.f;
    al_draw_filled_rounded_rectangle(x, y, x + size, y + size, r, r, white);
    if (number == 1) {
        al_draw_filled_circle(x + size * 0.5, y + size * 0.5, r, black);
    }
    else if (number == 2) {
        al_draw_filled_circle(x + size * 0.25, y + size * 0.75, r, black);
        al_draw_filled_circle(x + size * 0.75, y + size * 0.25, r, black);
    }
    else if (number == 3) {
        al_draw_filled_circle(x + size * 0.25, y + size * 0.75, r, black);
        al_draw_filled_circle(x + size * 0.5,  y + size * 0.5, r, black);
        al_draw_filled_circle(x + size * 0.75, y + size * 0.25, r, black);
    }
    else if (number == 4) {
        al_draw_filled_circle(x + size * 0.25, y + size * 0.25, r, black);
        al_draw_filled_circle(x + size * 0.25, y + size * 0.75, r, black);
        al_draw_filled_circle(x + size * 0.75, y + size * 0.25, r, black);
        al_draw_filled_circle(x + size * 0.75, y + size * 0.75, r, black);
    }
    else if (number == 5) {
        al_draw_filled_circle(x + size * 0.25, y + size * 0.25, r, black);
        al_draw_filled_circle(x + size * 0.25, y + size * 0.75, r, black);
        al_draw_filled_circle(x + size * 0.5,  y + size * 0.5, r, black);
        al_draw_filled_circle(x + size * 0.75, y + size * 0.25, r, black);
        al_draw_filled_circle(x + size * 0.75, y + size * 0.75, r, black);
    }
    else if (number == 6) {
        al_draw_filled_circle(x + size * 0.25, y + size * 0.25, r, black);
        al_draw_filled_circle(x + size * 0.25, y + size * 0.5, r, black);
        al_draw_filled_circle(x + size * 0.25, y + size * 0.75, r, black);
        al_draw_filled_circle(x + size * 0.75, y + size * 0.25, r, black);
        al_draw_filled_circle(x + size * 0.75, y + size * 0.5, r, black);
        al_draw_filled_circle(x + size * 0.75, y + size * 0.75, r, black);
    }
}

void draw_turn_info(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font) {
    int w = al_get_display_width(display);
    int size = MAP_CELL_SIZE / 2;
    float margin = 16.f;
    float line_height = al_get_font_line_height(font) / 2;
    //ALLEGRO_COLOR text_color = al_map_rgb(0, 0, 0);
    ALLEGRO_COLOR text_color = al_map_rgb(255, 255, 255);

    // clear space
    al_draw_filled_rectangle(w - 3.5 * size - 3 * margin, 0, w, size + (MAX_PLAYERS + 2) * margin + (MAX_PLAYERS + 2) * line_height, ResourceColor[WATER]);

    // draw dices
    draw_dice(display, font, w - margin - size, margin, size, dices[0]);
    draw_dice(display, font, w - 2 * (margin + size), margin, size, dices[1]);

    // draw text data
    float x = w - margin;
    float y = 1.5 * margin + size;
    char turn_text[32];
    sprintf_s(&turn_text, 32, "turn: %3d", turn_nr);
    al_draw_text(font, text_color, x - al_get_text_width(font, turn_text), y, 0, turn_text);
    y += margin + line_height;
    // player data
    for (int p = 0; p < MAX_PLAYERS; ++p) {
        float cr = line_height;
        al_draw_filled_circle(x - al_get_text_width(font, " player #X: XXX") - cr * 0.5, y + cr, cr, PlayerColors[p]);
        char player_text[32];
        sprintf_s(&player_text, 32, "%splayer #%d: %3d", current_player == p ? "* " : "", p + 1, player_points[p]);
        if (p == current_player) al_draw_filled_rounded_rectangle(x - al_get_text_width(font, player_text) - cr * 0.5, y, w + cr, y + line_height * 2, cr, cr, PlayerColors[current_player]);
        al_draw_text(font, text_color, x - al_get_text_width(font, player_text), y, 0, player_text);
        //al_draw_text(font, current_player == p ? text_color : PlayerColors[p], x - al_get_text_width(font, player_text), y, 0, player_text);
        //char cp_text[4];
        //_itoa_s(current_player + 1, cp_text, 4, 10);
        //al_draw_text(font, white, w - margin - al_get_text_width(font, cp_text) - cr * 0.5, 2.5 * margin + size + line_height, 0, cp_text);
        y += margin + line_height;
    }
}

void draw_cards(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, int shift, const int* cards, char* title, ALLEGRO_COLOR color) {
    ALLEGRO_COLOR black = al_map_rgb(0, 0, 0);
    ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);

    float padding = 8.f;
    float margin = 8.f;

    float line_height = al_get_font_line_height(font);
    float max_width = al_get_text_width(font, "bricks: XX"); // the longest resource word (hard coded for now :P)

    float x = padding + margin;
    float y = padding + margin + shift;
    float cr = line_height / 2;

    char text[32];
    sprintf_s(&text, 32, "%s:  ", title);
    al_draw_filled_rounded_rectangle(-cr, y, margin + al_get_text_width(font, text), y + line_height, cr, cr, color);
    al_draw_text(font, white, x, y, 0, text);
    y += margin + padding + line_height;
    x += margin;

    for (int i = 0; i < RESOURCES_COUNT; ++i) {
        char name[32];
        char count[4];
        //sprintf_s(&text, 32, "%s: %d", RESOURCES_NAMES[i], bank_cards[i]);
        _itoa_s(cards[i], count, 4, 10);
        sprintf_s(&name, 32, "%s:  ", RESOURCES_NAMES[i]);
        al_draw_filled_rounded_rectangle(margin + padding, y, max_width + 3 * margin + padding, y + line_height, margin, margin, ResourceColor[i + 2]);
        al_draw_text(font, black, x, y, 0, name);
        al_draw_text(font, black, x + max_width - al_get_text_width(font, count), y, 0, count);
        y += margin + line_height;
    }
}

void draw_bank_cards(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font)
{
    draw_cards(display, font, 0, &bank_cards, "bank cards", al_map_rgb(0, 0, 0));
}

void draw_player_cards(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font)
{
    float padding = 8.f;
    float margin = 8.f;

    float line_height = al_get_font_line_height(font);

    float shift = (line_height + margin) * (RESOURCES_COUNT + 1) + padding * 2;

    draw_cards(display, font, shift, &player_cards[current_player], "player cards", PlayerColors[current_player]);
}
