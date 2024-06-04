#ifndef MAP_H
#define MAP_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

// * types, structs, consts and enumerates
// resource related
typedef enum { DESERT, WOOD, BRICK, WHEAT, ORE, SHEEP } ResourceType;
extern ALLEGRO_COLOR ResourceColor[6];

// types
typedef struct { float x; float y; } POINT;
typedef struct { float x; float y; int value; ResourceType type; } HEX;
typedef struct { HEX* tiles; } BOARD;

// grid related
extern const int MAP_ROWS;
extern const int MAP_COLS;
extern const int MAP_CELL_SIZE;
extern const int MAP_ROAD_THICKNESS;
extern POINT  grid_offset;
extern POINT* grid;
extern POINT* grid_square;
extern POINT* grid_vertex;
extern BOARD  board;

// other
extern ALLEGRO_COLOR WaterColor;

// functions
void init_map(ALLEGRO_DISPLAY* display);
void free_map();
void draw_hex(HEX hex);
void draw_map();

#endif
