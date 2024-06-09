#include "ui.h"

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
        al_draw_filled_circle(x + size * 0.5, y + size * 0.5, r, black);
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
        al_draw_filled_circle(x + size * 0.5, y + size * 0.5, r, black);
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

void draw_turn_info(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, ALLEGRO_BITMAP* trophy, int* dices, int turn_nr, int current_player, int* player_points, int goal, const int MAX_PLAYERS, ALLEGRO_COLOR* PlayerColors) {
    int w = al_get_display_width(display);
    int size = MAP_CELL_SIZE / 2;
    float margin = 16.f;
    float line_height = al_get_font_line_height(font) / 2;
    //ALLEGRO_COLOR text_color = al_map_rgb(0, 0, 0);
    ALLEGRO_COLOR text_color = al_map_rgb(255, 255, 255);

    // clear space
    al_draw_filled_rectangle(w - 3.5 * size - 3. * margin, 0, w, size + (MAX_PLAYERS + 2) * margin + (MAX_PLAYERS + 4) * line_height, ResourceColor[WATER]);

    // draw dices
    draw_dice(display, font, w - margin - size, margin, size, dices[0]);
    draw_dice(display, font, w - 2 * (margin + size), margin, size, dices[1]);

    // draw text data
    float x = w - margin;
    float y = 1.5 * margin + size;
    char turn_text[32];
    sprintf_s(turn_text, 32, "turn: %3d", turn_nr);
    al_draw_text(font, text_color, x - al_get_text_width(font, turn_text), y, 0, turn_text);
    y += margin + line_height;
    // player data
    for (int p = 0; p < MAX_PLAYERS; ++p) {
        float cr = line_height;
        al_draw_filled_circle(x - al_get_text_width(font, " player #X: XX") - cr * 0.5, y + cr, cr, PlayerColors[p]);
        char player_text[32];
        sprintf_s(player_text, 32, "%splayer #%d: %2d", current_player == p ? "* " : "", p + 1, player_points[p]);
        if (p == current_player) al_draw_filled_rounded_rectangle(x - al_get_text_width(font, player_text) - cr, y, w + cr, y + line_height * 2, cr, cr, PlayerColors[current_player]);
        al_draw_text(font, text_color, x - al_get_text_width(font, player_text), y, 0, player_text);
        //al_draw_text(font, current_player == p ? text_color : PlayerColors[p], x - al_get_text_width(font, player_text), y, 0, player_text);
        //char cp_text[4];
        //_itoa_s(current_player + 1, cp_text, 4, 10);
        //al_draw_text(font, white, w - margin - al_get_text_width(font, cp_text) - cr * 0.5, 2.5 * margin + size + line_height, 0, cp_text);
        y += margin + line_height;
    }
    char goal_text[32];
    sprintf_s(goal_text, 32, "score goal: %2d", goal);
    al_draw_text(font, text_color, x - al_get_text_width(font, goal_text), y, 0, goal_text);
    float og_w = al_get_bitmap_width(trophy);
    float og_h = al_get_bitmap_height(trophy);
    al_draw_scaled_bitmap(trophy,
        0, 0, og_w, og_h,
        x - al_get_text_width(font, goal_text) - line_height * 2 - margin * 0.5, y, line_height * 2, line_height * 2,
        0);
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

    int cards_sum = 0;
    for (int i = 0; i < RESOURCES_COUNT; ++i) { cards_sum += cards[i]; }
    char text[32];
    sprintf_s(text, 32, "%s: %d", title, cards_sum);
    al_draw_filled_rounded_rectangle(-cr, y, 2 * margin + al_get_text_width(font, text) + cr, y + line_height, cr, cr, color);
    al_draw_text(font, white, x, y, 0, text);
    y += margin + padding + line_height;
    x += margin;

    for (int i = 0; i < RESOURCES_COUNT; ++i) {
        char name[32];
        char count[4];
        //sprintf_s(&text, 32, "%s: %d", RESOURCES_NAMES[i], bank_cards[i]);
        _itoa_s(cards[i], count, 4, 10);
        sprintf_s(name, 32, "%s:  ", RESOURCES_NAMES[i]);
        al_draw_filled_rounded_rectangle(margin + padding, y, max_width + 3 * margin + padding, y + line_height, margin, margin, ResourceColor[i + 2]);
        al_draw_text(font, black, x, y, 0, name);
        al_draw_text(font, black, x + max_width - al_get_text_width(font, count), y, 0, count);
        y += margin + line_height;
    }
}

void draw_other_players_cards(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, int current_player, const int MAX_PLAYERS, const int cards[4][5], ALLEGRO_COLOR* PlayerColors) {
    ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);
    
    float padding = 8.f;
    float margin = 8.f;

    float line_height = al_get_font_line_height(font);

    float shift = 2 * ((line_height + margin) * (RESOURCES_COUNT + 1) + padding * 2);
    float x = padding + margin;
    float y = padding + margin + shift;
    float cr = line_height / 2;

    for (int p = 0; p < MAX_PLAYERS; ++p) {
        if (p == current_player) continue;
        int cards_sum = 0;
        for (int i = 0; i < RESOURCES_COUNT; ++i) { cards_sum += cards[p][i]; }
        char text[32];
        sprintf_s(text, 32, "player #%d: %d", p + 1, cards_sum);
        al_draw_filled_rounded_rectangle(-cr, y, 2 * margin + al_get_text_width(font, text) + cr, y + line_height, cr, cr, PlayerColors[p]);
        al_draw_text(font, white, x, y, 0, text);
        y += margin + line_height;
    }
}

void draw_bank_cards(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, int* bank_cards)
{
    draw_cards(display, font, 0, bank_cards, "bank cards", al_map_rgb(0, 0, 0));
}

void draw_player_cards(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, int* player_cards, ALLEGRO_COLOR player_color)
{
    float padding = 8.f;
    float margin = 8.f;

    float line_height = al_get_font_line_height(font);

    float shift = (line_height + margin) * (RESOURCES_COUNT + 1) + padding * 2;

    draw_cards(display, font, shift, player_cards, "player cards", player_color);
}
