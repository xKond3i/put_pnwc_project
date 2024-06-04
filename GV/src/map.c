#include "map.h"

// resource related
ALLEGRO_COLOR ResourceColor[6];

// grid related
const int MAP_ROWS = 5;
const int MAP_COLS = 5;
const int MAP_CELL_SIZE = 80;
const int MAP_ROAD_THICKNESS = 8;
POINT  grid_offset;
POINT* grid;
POINT* grid_square;
POINT* grid_vertex;
BOARD  board;

// other
ALLEGRO_COLOR WaterColor;

void init_map(ALLEGRO_DISPLAY* display)
{
	ResourceColor[0] = al_map_rgb(225, 181, 104); // blank
	ResourceColor[1] = al_map_rgb(18, 150, 57);   // wood
	ResourceColor[2] = al_map_rgb(228, 104, 41);  // bricks
	ResourceColor[3] = al_map_rgb(241, 183, 25);  // wheat
	ResourceColor[4] = al_map_rgb(159, 165, 161); // ore
	ResourceColor[5] = al_map_rgb(142, 182, 12);  // sheep

	WaterColor = al_map_rgb(46, 128, 213);

	// generate grid
	srand(time(NULL));
	grid_offset.x = (al_get_display_width(display) - MAP_COLS * MAP_CELL_SIZE) / 2;
	grid_offset.y = (al_get_display_height(display) - MAP_ROWS * 5 / 6 * MAP_CELL_SIZE) / 2;
	grid = (POINT*)malloc(MAP_COLS * MAP_ROWS * sizeof(POINT));

	for (int y = 0; y < MAP_ROWS; ++y) {
		for (int x = 0; x < (MAP_COLS - y % 2); ++x) {
			int pos = y * MAP_COLS + x;
			if (y % 2 == 1) grid[pos].x = x * MAP_CELL_SIZE + (MAP_CELL_SIZE * 0.5) + grid_offset.x;
			else grid[pos].x = x * MAP_CELL_SIZE + grid_offset.x;
			grid[pos].y = (y * 0.75) * MAP_CELL_SIZE + grid_offset.y;
		}
	}

	board.tiles = (HEX*)malloc(MAP_COLS * MAP_ROWS * sizeof(HEX));
	grid_square = (POINT*)malloc(MAP_COLS * MAP_ROWS * sizeof(POINT));
	for (int y = 0; y < MAP_ROWS; ++y) {
		for (int x = 0; x < (MAP_COLS - y % 2); ++x) {
			int pos = y * MAP_COLS + x;
			if (y % 2 == 1) grid_square[pos].x = x * MAP_CELL_SIZE + (MAP_CELL_SIZE * 0.5);
			else grid_square[pos].x = x * MAP_CELL_SIZE;
			grid_square[pos].y = (y * 0.75) * MAP_CELL_SIZE;
			board.tiles[pos] = (HEX){ grid_square[pos].x, grid_square[pos].y, rand() % 12 + 1, rand() % 5 + 1 };
		}
	}
}

void free_map()
{
	free(grid);
	//free(board.tiles);
	//free(grid_square);
	//free(grid_vertex);
}

void draw_hex(HEX hex)
{
	float x = hex.x + grid_offset.x;
	float y = hex.y + grid_offset.y;

	float vertices[12] = {
		x + MAP_CELL_SIZE * 0.5, y,
		x + MAP_CELL_SIZE,       y + MAP_CELL_SIZE * 0.25,
		x + MAP_CELL_SIZE,       y + MAP_CELL_SIZE * 0.75,
		x + MAP_CELL_SIZE * 0.5, y + MAP_CELL_SIZE,
		x,						 y + MAP_CELL_SIZE * 0.75,
		x,						 y + MAP_CELL_SIZE * 0.25,
	};

	// bounding box
	// al_draw_rectangle(x, y, x + MAP_CELL_SIZE, y + MAP_CELL_SIZE, al_map_rgb(255, 0, 0), 1.0);

	// draw hex
	for (int i = 0; i < 6; i++) {
		int next = (i + 1) % 6;
		al_draw_filled_triangle(x + MAP_CELL_SIZE / 2, y + MAP_CELL_SIZE / 2, vertices[i * 2], vertices[i * 2 + 1], vertices[next * 2], vertices[next * 2 + 1], ResourceColor[hex.type]);
	}

	// road (outline)
	al_draw_polygon(vertices, 6, ALLEGRO_LINE_JOIN_BEVEL, ResourceColor[DESERT], MAP_ROAD_THICKNESS, 0.0);
}

void draw_map()
{
	al_clear_to_color(WaterColor);

	for (int y = 0; y < MAP_ROWS; ++y) {
		for (int x = 0; x < (MAP_COLS - y % 2); ++x) {
			int pos = y * MAP_COLS + x;
			draw_hex(board.tiles[pos]);
		}
	}
}
