#include "map.h"

// 🎨 design definitions
ALLEGRO_COLOR ResourceColor[7];

// map config
const int MAP_MIN_DIM = 5;
const int MAP_MAX_DIM = 11;
int MAP_ROWS; // DEFAULT 5
int MAP_COLS; // DEFAULT 5
int MAP_CELL_SIZE = 128; // DEFAULT 128
int MAP_ROAD_THICKNESS = 8; // DEFAULT 8

// grid config
int MAX_VERTICES;
int MAX_TILES;
POINT grid_offset;

// grid containers
POINT* grid_bounding;
POINT* grid_vertices;

// game board
char* terrain;
BOARD board;

// functionalities
bool load_map_from_file() {
	char* fname = "map.csv";
	FILE* f = NULL;
	errno_t err = fopen_s(&f, fname, "r");
	if (err != 0 || !f) return false;

	#if _DEBUG
	printf("loading map from file...\n\n");
	#endif

	// process file
	int id = 0, token_id = 0, MAX_LINE_LEN = (3 * MAP_MAX_DIM * MAP_MAX_DIM + 1) * sizeof(char);
	char* line = (char*)malloc(MAX_LINE_LEN);
	char* context = NULL;
	char* token;

	while (!feof(f))
	{
		fgets(line, MAX_LINE_LEN, f);
		line[strcspn(line, "\n")] = '\0';

		#if _DEBUG
		printf("line #%d: %s\n", id, line);
		#endif

		token_id = 0;
		token = strtok_s(line, ",", &context);

		while (token != NULL)
		{
			// process token
			if (id == 0) { // first line contains MAP_ROWS and MAP_COLS
				int num;
				// exception: too many header tokens
				if (token_id > 1) { printf("\033[31merr:\033[0m @ map (%s) - too many header arguments (just 2 expected)\n", fname); fclose(f); free(line); return false; }
				// exception: not a number
				if (!sscanf_s(token, "%d", &num)) { printf("\033[31merr:\033[0m @ map (%s) - failed to convert argument to a number\n", fname); fclose(f); free(line); return false; }
				// exception: out of range
				if (num < MAP_MIN_DIM || num > MAP_MAX_DIM) { printf("\033[31merr:\033[0m @ map (%s) - dimensions out of range <%d, %d>\n", fname, MAP_MIN_DIM, MAP_MAX_DIM); fclose(f); free(line); return false; }
				// assign values for MAP_ROWS and MAP_COLS
				if (token_id == 0)
					MAP_ROWS = num;
				else if (token_id == 1) {
					MAP_COLS = num;
					MAX_TILES = MAP_COLS * MAP_ROWS - (MAP_ROWS / 2) + 1; // ! remember that every second row has 1 tile less (add 1 to be safe)
					MAX_VERTICES = (MAP_COLS * 2 + 1) * (MAP_ROWS + 1) + 1;	 // ! for every bounding top line + bottom last = MAP_COLS * 2 + 1 vertices (edge case for odd number of rows)
					terrain = (char*)malloc(MAX_TILES * sizeof(char));
				}
			}
			else {
				if (id > MAP_ROWS) { printf("\033[31merr:\033[0m @ map (%s) - too many rows (declared %d in header)\n", fname, MAP_ROWS); fclose(f); free(line); return false; }
				if (id % 2 == 1 && token_id < MAP_COLS || id % 2 == 0 && token_id < MAP_COLS - 1) {
					int pos = (id - 1) * MAP_COLS + token_id - ((id - 1) / 2);
					if (strcmp(token, "W") == 0) { terrain[pos] = 'W'; }
					else if (strcmp(token, "R") == 0) { terrain[pos] = 'R'; }
					else if (strcmp(token, "0") == 0) { terrain[pos] = '0'; }
					else if (strcmp(token, "1") == 0) { terrain[pos] = '1'; }
					else if (strcmp(token, "2") == 0) { terrain[pos] = '2'; }
					else if (strcmp(token, "3") == 0) { terrain[pos] = '3'; }
					else if (strcmp(token, "4") == 0) { terrain[pos] = '4'; }
					else if (strcmp(token, "5") == 0) { terrain[pos] = '5'; }
					else {
						printf("\033[31merr:\033[0m @ map (%s) - invalid field type (not from WR012345)\n", fname);
						fclose(f);
						free(line);
						return false;
					}
				}
				else {
					printf("\033[31merr:\033[0m @ map (%s) - number of fields is incorrect\n", fname);
					fclose(f);
					free(line);
					return false;
				}
			}

			#if _DEBUG
			printf("token: %s\n", token);
			#endif

			token = strtok_s(NULL, ",", &context);
			++token_id;
		}

		#if _DEBUG
		printf("\n");
		#endif
		++id;
	}

	if (id <= MAP_ROWS) { printf("\033[31merr:\033[0m @ map (%s) - too few rows (declared %d in header)\n", fname, MAP_ROWS); fclose(f); free(line); return false; }

	// close and free memory
	fclose(f);
	free(line);
	return true;
}

void generate_map(ALLEGRO_DISPLAY* display) {
	// generate main grid (of bounding boxes) and assign tiles to board @ every grid spot
	int pos = 0;
	for (int row = 0; row < MAP_ROWS; ++row) {
		for (int col = 0; col < (MAP_COLS - row % 2); ++col) {
			if (row % 2 == 0) grid_bounding[pos].x = col * MAP_CELL_SIZE;						  // x for even rows
			else			  grid_bounding[pos].x = col * MAP_CELL_SIZE + (MAP_CELL_SIZE * 0.5); // x for odd rows (shift by half unit)
			grid_bounding[pos].y = (row * 0.75) * MAP_CELL_SIZE; // y changes by the same value for every row

			// add tile to board
			int type = rand() % 5 + 2; // DEFAULT (R)
			if (terrain[pos] == 'W') type = WATER;
			else if (terrain[pos] != 'R') type = (int)terrain[pos] - (int)'0' + 1;
			board.tiles[pos] = (HEX){ grid_bounding[pos].x, grid_bounding[pos].y, type, rand() % 12 + 1 };

			++pos;
		}
	}

	// generate submain grid (of vertices)
	pos = 0;
	float y = 0;
	for (int row = 0; row < MAP_ROWS + 1; ++row) {
		for (int col = 0; col < MAP_COLS; ++col) {
			if (row % 2 == 0) { // for even rows
				if (!(MAP_ROWS % 2 == 0 && row == MAP_ROWS && col == 0)) // edge case: even number of rows
					grid_vertices[++pos] = (POINT){ col * MAP_CELL_SIZE,				   y * MAP_CELL_SIZE + MAP_CELL_SIZE * 0.25 };
				grid_vertices[++pos] = (POINT){ col * MAP_CELL_SIZE + MAP_CELL_SIZE * 0.5, y * MAP_CELL_SIZE };
			}
			else { // for odd rows
				grid_vertices[++pos] = (POINT){ col * MAP_CELL_SIZE,					   y * MAP_CELL_SIZE - MAP_CELL_SIZE * 0.25 };
				grid_vertices[++pos] = (POINT){ col * MAP_CELL_SIZE + MAP_CELL_SIZE * 0.5, y * MAP_CELL_SIZE };
			}
		}

		// update y pos
		if (row % 2 == 0) { // for even rows
			if (!(MAP_ROWS % 2 == 0 && row == MAP_ROWS)) // edge case: even number of rows
				grid_vertices[++pos] = (POINT){ MAP_COLS * MAP_CELL_SIZE, y * MAP_CELL_SIZE + MAP_CELL_SIZE * 0.25 };
			y += 1;
		}
		else {
			grid_vertices[++pos] = (POINT){ MAP_COLS * MAP_CELL_SIZE, y * MAP_CELL_SIZE - MAP_CELL_SIZE * 0.25 };
			y += 0.5;
		}
	}
}

bool init_map(ALLEGRO_DISPLAY* display)
{
	// basic game values
	ResourceColor[0] = al_map_rgb(46, 128, 213);  // water
	ResourceColor[1] = al_map_rgb(225, 181, 104); // blank
	ResourceColor[2] = al_map_rgb(18, 150, 57);   // wood
	ResourceColor[3] = al_map_rgb(228, 104, 41);  // bricks
	ResourceColor[4] = al_map_rgb(241, 183, 25);  // wheat
	ResourceColor[5] = al_map_rgb(159, 165, 161); // ore
	ResourceColor[6] = al_map_rgb(142, 182, 12);  // sheep

	// map properties
	if (!load_map_from_file()) return false;

	grid_offset.x = (al_get_display_width(display)  - MAP_COLS * MAP_CELL_SIZE) / 2;		 // remaining horizontal space / 2
	grid_offset.y = (al_get_display_height(display) - MAP_ROWS * MAP_CELL_SIZE * 5 / 6) / 2; // remaining vertical space / 2

	// allocate memory for grid containers and board
	grid_bounding = (POINT*)malloc(MAX_TILES * sizeof(POINT));
	grid_vertices = (POINT*)malloc(MAX_VERTICES * sizeof(POINT));

	board.tiles = (HEX*)malloc(MAX_TILES * sizeof(HEX));
	board.placements = (PLACEMENT*)malloc(MAX_VERTICES * sizeof(PLACEMENT));

	generate_map(display);

	return true;
}

void free_map()
{
	free(terrain);
	free(board.tiles);
	free(board.placements);
	free(board.roads);
	free(grid_bounding);
	free(grid_vertices);
}

void draw_hex(HEX hex, ALLEGRO_FONT* font)
{
	if (hex.type == WATER) return;

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

	// draw road (outline)
	al_draw_polygon(vertices, 6, ALLEGRO_LINE_JOIN_BEVEL, ResourceColor[DESERT], MAP_ROAD_THICKNESS, 0.0);
}

#if _DEBUG
void draw_bounding_box(HEX hex) {
	//if (hex.type == WATER) return;

	float x = hex.x + grid_offset.x;
	float y = hex.y + grid_offset.y;
	al_draw_rectangle(x, y, x + MAP_CELL_SIZE, y + MAP_CELL_SIZE, al_map_rgb(255, 0, 0), 1.0);
}
#endif

void draw_map(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font)
{
	al_clear_to_color(ResourceColor[WATER]);

	// draw tiles
	for (int pos = 0; pos < MAX_TILES - 1; ++pos) {
		draw_hex(board.tiles[pos], font);
	}

	#if _DEBUG
	// [DEBUG] SHOW SQUARE BOUNDING BOXES
	for (int i = 0; i < MAX_TILES; ++i)
		draw_bounding_box(board.tiles[i]);

	// [DEBUG] SHOW ALL VERTICES
	for (int i = 0; i < MAX_VERTICES; ++i)
		al_draw_filled_circle(grid_vertices[i].x + grid_offset.x, grid_vertices[i].y + grid_offset.y, 8, al_map_rgb(255, 0, 0));
	#endif
}
