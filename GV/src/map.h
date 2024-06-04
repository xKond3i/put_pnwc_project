#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

// * types, structs, consts and enumerates
// resource related
typedef enum { DESERT, WOOD, BRICK, WHEAT, ORE, SHEEP } ResourceType;
extern ALLEGRO_COLOR ResourceColor[6];

// types
typedef struct { float x; float y; } POINT;
typedef struct { float x; float y; int player; int building; } PLACEMENT;
typedef struct { bool active; float x; float y; int value; ResourceType type; } HEX;
typedef struct { HEX* tiles; PLACEMENT* vertices; } BOARD;

// grid related
extern const int MAP_ROWS;
extern const int MAP_COLS;
extern const int MAP_CELL_SIZE;
extern const int MAP_ROAD_THICKNESS;
extern int MAX_VERTICES;
extern int MAX_TILES;
extern POINT  grid_offset;
extern POINT* grid_square;
extern POINT* grid_vertex;
extern BOARD  board;

// other
extern ALLEGRO_COLOR WaterColor;

// functions
void init_map(ALLEGRO_DISPLAY* display);
void free_map();
void generate_map(ALLEGRO_DISPLAY* display);
void draw_hex(HEX hex, ALLEGRO_FONT* font);
void draw_map(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font);

#if _DEBUG
void draw_bounding_box(HEX hex);
#endif
