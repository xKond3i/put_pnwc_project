#pragma once

// ! worth looking: https://stackoverflow.com/a/16874550
// ! worth looking: https://www.redblobgames.com/grids/hexagons

// debug extras
#if _DEBUG
#define _DEBUG_SHOW_GRIDS
#define _DEBUG_FILE_EXTRA_OUT
#define _DEBUG_GRID_EXTRA_OUT
#endif

// includes
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

// resources enumerations
typedef enum { WATER, DESERT, WOOD, BRICK, WHEAT, ORE, SHEEP } ResourceType;
typedef enum { NONE, SETTLEMENT, CITY } PlacementType;
typedef enum { NOONE, P1, P2, P3, P4 } Player;

extern const int RESOURCES_COUNT;
extern const char RESOURCES_NAMES[5][10];

// 🎨 design definitions
extern ALLEGRO_COLOR ResourceColor[7];

// types
typedef struct { float x; float y; } POINT;
typedef struct { float x; float y; ResourceType type; int value; } HEX;
typedef struct { float x; float y; int player; PlacementType building; int neighbors[3]; int parents[3]; bool active; } PLACEMENT;
typedef struct { POINT start; POINT end; Player player; } ROADS;
typedef struct { HEX* tiles; PLACEMENT* placements; ROADS* roads; } BOARD;

// map config
extern const int MAP_MIN_DIM;
extern const int MAP_MAX_DIM;
extern int MAP_ROWS;
extern int MAP_COLS;
extern int MAP_CELL_SIZE;
extern int MAP_ROAD_THICKNESS;

// grid config
extern int MAX_VERTICES;
extern int MAX_TILES;
extern POINT grid_offset;

// grid containers
extern POINT* grid_bounding;
extern POINT* grid_vertices;

// game board
extern char* terrain;
extern BOARD board;

// functionalities
void free_map();
bool init_map(ALLEGRO_DISPLAY* display);
bool load_map_from_file(char* fname, bool extra_debug);
void generate_map(ALLEGRO_DISPLAY* display);
void draw_hex(HEX hex, ALLEGRO_FONT* font);
void draw_map(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font);
void draw_vertices(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, int selected);

// DEBUG functionalities
#ifdef _DEBUG_SHOW_GRIDS
	void draw_bounding_box(HEX hex);
#endif
#ifdef _DEBUG_GRID_EXTRA_OUT
	void print_vertex_data(int pos, char* title, int p0, int p1, int p2, int n0, int n1, int n2);
#endif
