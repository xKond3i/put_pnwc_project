#include "map.h"

// resource related
ALLEGRO_COLOR ResourceColor[6];

// grid related
const int MAP_ROWS = 5;
const int MAP_COLS = 5;
const int MAP_CELL_SIZE = 128;
const int MAP_ROAD_THICKNESS = 8;
int MAX_TILES;
int MAX_VERTICES;
POINT  grid_offset;
POINT* grid_square;
POINT* grid_vertex;
BOARD  board;

// other
ALLEGRO_COLOR WaterColor;

void generate_map(ALLEGRO_DISPLAY* display) {
	// generate also all the necessary grids
	int pos = 0, vertpos = 0;
	for (int y = 0; y < MAP_ROWS; ++y) {
		for (int x = 0; x < (MAP_COLS - y % 2); ++x) {
			// @grid_square: add bounding boxes
			if (y % 2 == 1) grid_square[pos].x = x * MAP_CELL_SIZE + (MAP_CELL_SIZE * 0.5);
			else grid_square[pos].x = x * MAP_CELL_SIZE;
			grid_square[pos].y = (y * 0.75) * MAP_CELL_SIZE;

			// @grid_vertex: add verticies
			if (y % 2 == 0) {
				grid_vertex[++vertpos] = (POINT){ grid_square[pos].x + MAP_CELL_SIZE * 0.5, grid_square[pos].y };
				grid_vertex[++vertpos] = (POINT){ grid_square[pos].x,						grid_square[pos].y + MAP_CELL_SIZE * 0.25 };
				grid_vertex[++vertpos] = (POINT){ grid_square[pos].x,						grid_square[pos].y + MAP_CELL_SIZE * 0.75 };
				grid_vertex[++vertpos] = (POINT){ grid_square[pos].x + MAP_CELL_SIZE * 0.5, grid_square[pos].y + MAP_CELL_SIZE };
			}
			else if (y == MAP_ROWS - 1 && y % 2 == 1) {
				// edge case - map has even number of rows
				grid_vertex[++vertpos] = (POINT){ grid_square[pos].x,						grid_square[pos].y + MAP_CELL_SIZE * 0.75 };
				grid_vertex[++vertpos] = (POINT){ grid_square[pos].x + MAP_CELL_SIZE * 0.5, grid_square[pos].y + MAP_CELL_SIZE };
			}

			// @board: add tiles
			board.tiles[pos] = (HEX){ true, grid_square[pos].x, grid_square[pos].y, rand() % 12 + 1, rand() % 5 + 1 };

			// map specific (shaping the map)
			float r = MAP_ROWS * MAP_CELL_SIZE * 5 / 12;
			float center_x = al_get_display_width(display) / 2;
			float center_y = al_get_display_height(display) / 2;
			float grid_center_x = grid_square[pos].x + MAP_CELL_SIZE / 2 + grid_offset.x;
			float grid_center_y = grid_square[pos].y + MAP_CELL_SIZE / 2 + grid_offset.y;
			float diff_x = fabs(center_x - grid_center_x);
			float diff_y = fabs(center_y - grid_center_y);
			al_draw_circle(center_x, center_y, r, al_map_rgb(255, 0, 0), 1.0);
			if (diff_x * diff_x + diff_y * diff_y > r * r) board.tiles[pos].active = false;

			// increase tile pos
			++pos;
		}

		// add two remaining vertices
		if (y % 2 == 0) {
			grid_vertex[++vertpos] = (POINT){ grid_square[pos - 1].x + MAP_CELL_SIZE, grid_square[pos - 1].y + MAP_CELL_SIZE * 0.25 };
			grid_vertex[++vertpos] = (POINT){ grid_square[pos - 1].x + MAP_CELL_SIZE, grid_square[pos - 1].y + MAP_CELL_SIZE * 0.75 };
		}
		else if (y == MAP_ROWS - 1 && y % 2 == 1) {
			// edge case - map has even number of rows
			grid_vertex[++vertpos] = (POINT){ grid_square[pos - 1].x + MAP_CELL_SIZE, grid_square[pos - 1].y + MAP_CELL_SIZE * 0.75 };
		}
	}

	// @vertex_grid: we need to disable verticies that are own by only inactive tiles
	for (int i = 0; i < MAX_VERTICES; ++i) {

	}

	// map specific (shaping the map)
	board.tiles[MAX_TILES / 2 - 1].type = DESERT;
}

void init_map(ALLEGRO_DISPLAY* display)
{
	// BASIC GAME VALUES
	ResourceColor[0] = al_map_rgb(225, 181, 104); // blank
	ResourceColor[1] = al_map_rgb(18, 150, 57);   // wood
	ResourceColor[2] = al_map_rgb(228, 104, 41);  // bricks
	ResourceColor[3] = al_map_rgb(241, 183, 25);  // wheat
	ResourceColor[4] = al_map_rgb(159, 165, 161); // ore
	ResourceColor[5] = al_map_rgb(142, 182, 12);  // sheep

	WaterColor = al_map_rgb(46, 128, 213);

	// * GENERATE GRIDS AND MAP
	srand(time(NULL));

	MAX_TILES = MAP_COLS * MAP_ROWS - (MAP_ROWS / 2) + 1;
	MAX_VERTICES = (4 * MAP_COLS + 2) * ((MAP_ROWS / 2) + 1) + 1;

	grid_offset.x = (al_get_display_width(display) - MAP_COLS * MAP_CELL_SIZE) / 2;
	grid_offset.y = (al_get_display_height(display) - MAP_ROWS * 5 / 6 * MAP_CELL_SIZE) / 2;

	board.tiles = (HEX*)malloc(MAX_TILES * sizeof(HEX));
	grid_square = (POINT*)malloc(MAX_TILES * sizeof(POINT));
	grid_vertex = (POINT*)malloc(MAX_VERTICES * sizeof(POINT));

	generate_map(display);
}

void free_map()
{
	free(board.tiles);
	free(board.vertices);
	free(grid_square);
	free(grid_vertex);
}

void draw_hex(HEX hex, ALLEGRO_FONT* font)
{
	if (hex.active == false) return;

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

	// draw hex
	for (int i = 0; i < 6; i++) {
		int next = (i + 1) % 6;
		al_draw_filled_triangle(x + MAP_CELL_SIZE / 2, y + MAP_CELL_SIZE / 2, vertices[i * 2], vertices[i * 2 + 1], vertices[next * 2], vertices[next * 2 + 1], ResourceColor[hex.type]);
	}

	// draw hex dice value
	if (hex.type != DESERT) {
		char str[4];
		_itoa_s(hex.value, str, 4, 10);
		al_draw_filled_rounded_rectangle(x + MAP_CELL_SIZE / 4, y + MAP_CELL_SIZE / 2 - MAP_CELL_SIZE / 8, x + MAP_CELL_SIZE * 3/4, y + MAP_CELL_SIZE / 2 + MAP_CELL_SIZE / 8, 8, 8, al_map_rgb(255, 255, 255));
		al_draw_text(font, al_map_rgb(0, 0, 0), x + MAP_CELL_SIZE / 2, y + MAP_CELL_SIZE / 2 - MAP_CELL_SIZE / 7, ALLEGRO_ALIGN_CENTER, str);
	}

	// road (outline)
	al_draw_polygon(vertices, 6, ALLEGRO_LINE_JOIN_BEVEL, ResourceColor[DESERT], MAP_ROAD_THICKNESS, 0.0);
}

#if _DEBUG
void draw_bounding_box(HEX hex) {
	if (hex.active == false) return;

	float x = hex.x + grid_offset.x;
	float y = hex.y + grid_offset.y;
	al_draw_rectangle(x, y, x + MAP_CELL_SIZE, y + MAP_CELL_SIZE, al_map_rgb(255, 0, 0), 1.0);
}
#endif

void draw_map(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font)
{
	al_clear_to_color(WaterColor);

	// draw tiles
	for (int pos = 0; pos < MAP_COLS * MAP_ROWS - (MAP_ROWS/2); ++pos) {
		draw_hex(board.tiles[pos], font);
	}

	#if _DEBUG
	// [DEBUG] SHOW SQUARE BOUNDING BOXES
	for (int i = 0; i < MAX_TILES; ++i)
		draw_bounding_box(board.tiles[i]);

	// [DEBUG] SHOW ALL VERTICES
	for (int i = 0; i < MAX_VERTICES; ++i)
		al_draw_filled_circle(grid_vertex[i].x + grid_offset.x, grid_vertex[i].y + grid_offset.y, 8, al_map_rgb(255, 0, 0));

	// show radius, that cuts hexes from use
	float r = MAP_ROWS * MAP_CELL_SIZE * 5 / 12;
	float center_x = al_get_display_width(display) / 2;
	float center_y = al_get_display_height(display) / 2;
	al_draw_circle(center_x, center_y, r, al_map_rgb(255, 0, 0), 1.0);
	#endif
}
