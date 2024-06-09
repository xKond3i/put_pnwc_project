#include "game.h"

int selected = -1;

// bank info
const int MAX_TYPE_CARDS = 19;
int bank_cards[5];

// player info
ALLEGRO_COLOR PlayerColors[4];
const int MAX_PLAYERS = 4;
int player_cards[4][5];
int player_points[4];

// turn info
int current_player;
int turn_nr;
int dices[2];

// winning conditions
int goal = 10;

bool init_game(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font)
{
    if (!init_map(display)) return false;

    PlayerColors[0] = al_map_rgb(196, 0, 0);
    PlayerColors[1] = al_map_rgb(0, 0, 196);
    PlayerColors[2] = al_map_rgb(0, 128, 0);
    PlayerColors[3] = al_map_rgb(128, 0, 196);

    restart_game(display, font);

    return true;
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
void update_loop(ALLEGRO_EVENT* event, ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, ALLEGRO_BITMAP* trophy)
{
    // update loop (when timer ticks)
    if (event->type == ALLEGRO_EVENT_TIMER) {
        if (map_update_counter == 0) draw_map(display, font, turn_nr <= MAX_PLAYERS ? -1 : dices[0] + dices[1]);
        map_update_counter = (map_update_counter + 1) % 30;

        draw_vertices(display, font, selected, current_player, PlayerColors);
    }

    // ui
    draw_turn_info(display, font, trophy, dices, turn_nr, current_player, player_points, goal, MAX_PLAYERS, PlayerColors);

    draw_bank_cards(display, font, &bank_cards);
    draw_player_cards(display, font, &player_cards[current_player], PlayerColors[current_player]);

    if (turn_nr <= 2 * MAX_PLAYERS) {
        float margin = 16.f;
        float w = al_get_display_width(display);
        float h = al_get_display_height(display);
        float lh = al_get_font_line_height(font);
        char text[] = "* placement choosing phase *";
        al_draw_filled_rectangle(0, h - margin - lh, margin + al_get_text_width(font, text), h, ResourceColor[WATER]);
        al_draw_text(font, al_map_rgb(255, 0, 0), margin, h - margin - lh, 0, text);
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
        
        if (board.placements[idx].player == NOONE || board.placements[idx].player == current_player) {
            selected = idx;
        }
    }
    else if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        if (event->mouse.button == 1) {
            if (board.placements[selected].player == NOONE) {
                // first turns - free stuff
                bool first_turns = turn_nr <= (2 * MAX_PLAYERS);
                // check if player has resources for a house
                bool has_resources = true;
                if (player_cards[current_player][WOOD] < 1 || player_cards[current_player][BRICK] < 1 || player_cards[current_player][WHEAT] < 1 || player_cards[current_player][SHEEP] < 1) has_resources = false;
                PLACEMENT v = board.placements[selected];
                // check if neighbors are free
                bool neighbors_free = true;
                for (int i = 0; i < 3; ++i) {
                    if (board.placements[v.neighbors[i]].player != NOONE) neighbors_free = false;
                }
                if (neighbors_free && (has_resources || first_turns)) {
                    board.placements[selected].building = TENT;
                    board.placements[selected].player = current_player;
                    player_points[current_player]++;
                    next_turn();
                }
            }
        }
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
    selected = -1;
}
