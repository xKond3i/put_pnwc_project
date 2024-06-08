#include "map.h"

// resources
const int RESOURCES_COUNT = 5;
const char RESOURCES_NAMES[5][10] = {"wood", "bricks", "wheat", "ore", "sheep"};

// 🎨 design definitions
ALLEGRO_COLOR ResourceColor[7];

// map config
const int MAP_MIN_DIM = 3;
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
bool load_map_from_file(char* fname) {
	FILE* f = NULL;
	errno_t err = fopen_s(&f, fname, "r");
	if (err != 0 || !f) return false;

	#ifdef _DEBUG_FILE_EXTRA_OUT
		printf("\033[33m[FILE] starting...\033[0m\n\n");
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

		// encountered last empty line (ignore)
		if (strlen(line) <= 1 && id > MAP_ROWS) {
			fclose(f);
			free(line);
			return true;
		}

		#ifdef _DEBUG_FILE_EXTRA_OUT
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
					MAX_VERTICES = (MAP_COLS * 2 + 1) * (MAP_ROWS + 1);	  // ! for every bounding top line + bottom last = MAP_COLS * 2 + 1 vertices
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

			#ifdef _DEBUG_FILE_EXTRA_OUT
				printf("token: %s\n", token);
			#endif

			token = strtok_s(NULL, ",", &context);
			++token_id;
		}

		#ifdef _DEBUG_FILE_EXTRA_OUT
			printf("\n");
		#endif
		++id;
	}

	if (id <= MAP_ROWS) { printf("\033[31merr:\033[0m @ map (%s) - too few rows (declared %d in header)\n", fname, MAP_ROWS); fclose(f); free(line); return false; }

	#ifdef _DEBUG_FILE_EXTRA_OUT
		printf("\033[33m[FILE] finished.\033[0m\n\n");
	#endif

	// close and free memory
	fclose(f);
	free(line);
	return true;
}

void generate_map(ALLEGRO_DISPLAY* display) {
	#ifdef _DEBUG_GRID_EXTRA_OUT
		printf("\033[33m[MAP] starting...\033[0m\n\n");
	#endif

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
			int value = (type == WATER || type == DESERT) ? -1 : rand() % 11 + 2;
			board.tiles[pos] = (HEX){ grid_bounding[pos].x, grid_bounding[pos].y, type, value };

			pos++;
		}
	}

	// generate submain grid (of vertices)
	pos = 0;
	float y = 0;
	for (int row = 0; row < MAP_ROWS + 1; ++row) {
		for (int col = 0; col < MAP_COLS; ++col) {
			if (row % 2 == 0) { // for even rows
				grid_vertices[pos++] = (POINT){ col * MAP_CELL_SIZE,				   y * MAP_CELL_SIZE + MAP_CELL_SIZE * 0.25 };
				grid_vertices[pos++] = (POINT){ col * MAP_CELL_SIZE + MAP_CELL_SIZE * 0.5, y * MAP_CELL_SIZE };
			}
			else { // for odd rows
				grid_vertices[pos++] = (POINT){ col * MAP_CELL_SIZE,					   y * MAP_CELL_SIZE - MAP_CELL_SIZE * 0.25 };
				grid_vertices[pos++] = (POINT){ col * MAP_CELL_SIZE + MAP_CELL_SIZE * 0.5, y * MAP_CELL_SIZE };
			}
		}

		// update y pos
		if (row % 2 == 0) { // for even rows
			if (!(MAP_ROWS % 2 == 0 && row == MAP_ROWS)) // edge case: even number of rows
				grid_vertices[pos++] = (POINT){ MAP_COLS * MAP_CELL_SIZE, y * MAP_CELL_SIZE + MAP_CELL_SIZE * 0.25 };
			y += 1;
		}
		else {
			grid_vertices[pos++] = (POINT){ MAP_COLS * MAP_CELL_SIZE, y * MAP_CELL_SIZE - MAP_CELL_SIZE * 0.25 };
			y += 0.5;
		}
	}

	// add vertices to board and flag if they are interactive or not
	int VERTICES_IN_ROW = (MAP_COLS * 2 + 1);
	for (int pos = 0; pos < MAX_VERTICES; ++pos) {
		//printf("v[%d] @ (%g, %g)\n", pos, grid_vertices[pos].x, grid_vertices[pos].y);
		int row = pos / VERTICES_IN_ROW;
		int col = pos - row * VERTICES_IN_ROW;

		// each vertex belongs to max 3 vertices (check if all are water, if so turn it into noninteractive)
		int parents[3] = { -1, -1, -1 };
		int neighbors[3] = { -1, -1, -1 };

		// ! edge cases
		// first column
		if (col == 0 && MAP_ROWS % 2 == 1) {
			// parents
			if (row % 2 == 0) { parents[0] = row * MAP_COLS - (row / 2); }
			else { parents[0] = (row - 1) * MAP_COLS - (row / 2); }

			// neighbors
			if (pos % 2 == 0) neighbors[0] = pos + VERTICES_IN_ROW; // bottom
			else neighbors[0] = pos - VERTICES_IN_ROW; // top
			neighbors[1] = pos + 1; // right

			#ifdef _DEBUG_GRID_EXTRA_OUT
				print_vertex_data(pos, "first column", parents[0], parents[1], parents[2], neighbors[0], neighbors[1], neighbors[2]);
			#endif
		}
		// first column (edge case for even number of rows - last row has 1 tile less)
		else if (col == 0 && MAP_ROWS % 2 == 0) {
			if (row != MAP_ROWS) {
				// parents
				if (row % 2 == 0) { parents[0] = row * MAP_COLS - (row / 2); }
				else { parents[0] = (row - 1) * MAP_COLS - (row / 2); }

				// neighbors
				if (pos % 2 == 0) neighbors[0] = pos + VERTICES_IN_ROW; // bottom
				else neighbors[0] = pos - VERTICES_IN_ROW; // top
				neighbors[1] = pos + 1; // right
			}

			#ifdef _DEBUG_GRID_EXTRA_OUT
				print_vertex_data(pos, "first column (edge case: even number of rows)", parents[0], parents[1], parents[2], neighbors[0], neighbors[1], neighbors[2]);
			#endif
		}
		// last column
		else if (col == VERTICES_IN_ROW - 1 && MAP_ROWS % 2 == 1) {
			// parents
			if (row % 2 == 0) { parents[0] = (row + 1) * MAP_COLS - (row / 2) - 1; }
			else { parents[0] = (row) * MAP_COLS - (row / 2) - 1; }

			// neighbors
			if (pos % 2 == 0) neighbors[0] = pos + VERTICES_IN_ROW; // bottom
			else neighbors[0] = pos - VERTICES_IN_ROW; // top
			neighbors[1] = pos - 1; // left

			#ifdef _DEBUG_GRID_EXTRA_OUT
				print_vertex_data(pos, "last column", parents[0], parents[1], parents[2], neighbors[0], neighbors[1], neighbors[2]);
			#endif
		}
		// last column (edge case for even number of rows - last row has 1 tile less)
		else if (col == VERTICES_IN_ROW - 1 && MAP_ROWS % 2 == 0) {
			if (row != MAP_ROWS) {
				// parents
				if (row % 2 == 0) { parents[0] = (row + 1) * MAP_COLS - (row / 2) - 1; }
				else { parents[0] = (row)*MAP_COLS - (row / 2) - 1; }

				// neighbors
				if (pos % 2 == 0) neighbors[0] = pos + VERTICES_IN_ROW; // bottom
				else neighbors[0] = pos - VERTICES_IN_ROW; // top
				neighbors[1] = pos - 1; // left

			}
			
			#ifdef _DEBUG_GRID_EXTRA_OUT
				print_vertex_data(pos, "last column (edge case: even number of rows)", parents[0], parents[1], parents[2], neighbors[0], neighbors[1], neighbors[2]);
			#endif
		}
		// first row
		else if (row == 0) {
			if (pos % 2 == 0) {
				// parents
				parents[0] = (col / 2) - 1; // left
				parents[1] = (col / 2); // right

				// neighbors
				neighbors[0] = pos + VERTICES_IN_ROW; // bottom
				neighbors[1] = pos - 1; // left
				neighbors[2] = pos + 1; // right
			}
			else {
				// parents
				parents[0] = col / 2;

				// neighbors
				neighbors[0] = pos - 1; // left
				neighbors[1] = pos + 1; // right
			}

			#ifdef _DEBUG_GRID_EXTRA_OUT
				print_vertex_data(pos, "first row", parents[0], parents[1], parents[2], neighbors[0], neighbors[1], neighbors[2]);
			#endif
		}
		// last row
		else if (row == MAP_ROWS && MAP_ROWS % 2 == 1) {
			if (pos % 2 == 1) {
				// parents
				parents[0] = (row - 1) * MAP_COLS + (col / 2) - (row / 2) - 1; // left
				parents[1] = (row - 1) * MAP_COLS + (col / 2) - (row / 2); // right

				// neighbors
				neighbors[0] = pos - VERTICES_IN_ROW; // top
				if (col != 1 && row != 0) neighbors[1] = pos - 1; // left
				if (col != VERTICES_IN_ROW - 2 && row != 0) neighbors[2] = pos + 1; // right
			}
			else {
				// parents
				parents[0] = (row - 1) * MAP_COLS + (col / 2) - (row / 2); // top

				// neighbors
				neighbors[0] = pos - 1; // left
				neighbors[1] = pos + 1; // right
			}

			#ifdef _DEBUG_GRID_EXTRA_OUT
				print_vertex_data(pos, "last row", parents[0], parents[1], parents[2], neighbors[0], neighbors[1], neighbors[2]);
			#endif
		}
		// last row (edge case for even number of rows - last row has 1 tile less)
		else if (row == MAP_ROWS && MAP_ROWS % 2 == 0) {
			if (pos % 2 == 1) {
				// parents
				if (col > 2) parents[0] = (row - 1) * MAP_COLS + (col / 2) - (row / 2);
				if (col < VERTICES_IN_ROW - 2) parents[1] = (row - 1) * MAP_COLS + (col / 2) - (row / 2) + 1;

				// neighbors
				neighbors[0] = pos - VERTICES_IN_ROW; // top
				if (col > 1) neighbors[1] = pos - 1; // left
				if (col < VERTICES_IN_ROW - 2) neighbors[2] = pos + 1; // right
			}
			else {
				// parents
				parents[0] = (row - 1) * MAP_COLS + (col / 2) - (row / 2);

				// neighbors
				if (col > 1) neighbors[0] = pos - 1; // left
				if (col < VERTICES_IN_ROW - 2) neighbors[1] = pos + 1; // right
			}

			#ifdef _DEBUG_GRID_EXTRA_OUT
				print_vertex_data(pos, "last row (edge case: even number of rows)", parents[0], parents[1], parents[2], neighbors[0], neighbors[1], neighbors[2]);
			#endif
		}
		// most cases (in the center)
		// even vertex: bottom, top, right hexes
		else if (pos % 2 == 0) {
			// parents
			parents[0] = (row-1) * MAP_COLS + (col / 2) - (row / 2); // top
			if (col != 1) parents[1] = parents[0] + MAP_COLS - 1; // left
			if (col != VERTICES_IN_ROW - 2) parents[2] = parents[0] + MAP_COLS; // right

			// neighbors
			neighbors[0] = pos + VERTICES_IN_ROW; // bottom
			neighbors[1] = pos - 1; // left
			neighbors[2] = pos + 1; // right

			#ifdef _DEBUG_GRID_EXTRA_OUT
				print_vertex_data(pos, "most cases (even)", parents[0], parents[1], parents[2], neighbors[0], neighbors[1], neighbors[2]);
			#endif
		}
		// most cases (in the center)
		// odd vertex: bottom, left, right hexes
		else if (pos % 2 == 1) {
			parents[0] = row * MAP_COLS + (col / 2) - (row / 2) - (row % 2); // bottom
			if (col != 1) parents[1] = parents[0] - MAP_COLS; // left
			if (col != VERTICES_IN_ROW - 2) parents[2] = parents[0] - MAP_COLS + 1; // right

			// neighbors
			neighbors[0] = pos - VERTICES_IN_ROW; // top
			neighbors[1] = pos - 1; // left
			neighbors[2] = pos + 1; // right

			#ifdef _DEBUG_GRID_EXTRA_OUT
				print_vertex_data(pos, "most cases (odd)", parents[0], parents[1], parents[2], neighbors[0], neighbors[1], neighbors[2]);
			#endif
		}

		// check if all parents are water, if so disable
		bool is_active = false;
		for (int i = 0; i < 3; ++i) {
			if (parents[i] == -1) continue;
			if (board.tiles[parents[i]].type != WATER) { is_active = true; break; }
		}

		board.placements[pos] = (PLACEMENT){ grid_vertices[pos].x, grid_vertices[pos].y, 0, NONE };
		board.placements[pos].active = is_active;
		//board.placements[pos].neighbors = neighbors;
		// array passing seems to not work...
		board.placements[pos].neighbors[0] = neighbors[0];
		board.placements[pos].neighbors[1] = neighbors[1];
		board.placements[pos].neighbors[2] = neighbors[2];
		//board.placements[pos].parents = parents;
		// array passing seems to not work...
		board.placements[pos].parents[0] = parents[0];
		board.placements[pos].parents[1] = parents[1];
		board.placements[pos].parents[2] = parents[2];
	}

	#ifdef _DEBUG_FILE_EXTRA_OUT
		printf("\n");
	#endif

	// remove neighbors that are not interactive
	for (int pos = 0; pos < MAX_VERTICES; ++pos) {
		if (board.placements[pos].active == true) continue; // skip active vertices
		#ifdef _DEBUG_GRID_EXTRA_OUT
			printf("v[%d] is is not owned by any resource hex.\n", pos);
		#endif

		// look for in-active vertex neighbors
		for (int i = 0; i < 3; ++i) {
			int neighbor_id = board.placements[pos].neighbors[i];
			if (neighbor_id == -1) continue; // skip non-existent neighbors

			// remove in-active vertex from neighbors
			for (int j = 0; j < 3; ++j) {
				if (board.placements[neighbor_id].neighbors[j] != pos) continue;

				board.placements[neighbor_id].neighbors[j] = -1;
				#ifdef _DEBUG_GRID_EXTRA_OUT
					printf("   thus is no longer a neighbor of v[%d]\n", neighbor_id);
				#endif
			}
		}
	}

	#ifdef _DEBUG_GRID_EXTRA_OUT
		printf("\n\033[33m[MAP] finished.\033[0m\n");
	#endif
}

bool init_map(ALLEGRO_DISPLAY* display)
{
	// basic game values
	ResourceColor[0] = al_map_rgb(46, 128, 213);  // water
	//ResourceColor[1] = al_map_rgb(174, 139, 80);  // desert
	ResourceColor[1] = al_map_rgb(225, 181, 104); // desert
	ResourceColor[2] = al_map_rgb(18, 150, 57);   // wood
	ResourceColor[3] = al_map_rgb(228, 104, 41);  // bricks
	ResourceColor[4] = al_map_rgb(241, 183, 25);  // wheat
	ResourceColor[5] = al_map_rgb(159, 165, 161); // ore
	ResourceColor[6] = al_map_rgb(142, 182, 12);  // sheep

	// map properties
	if (!load_map_from_file("map.csv")) return false;

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

void draw_hex(HEX hex, ALLEGRO_FONT* font, bool overlay)
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
		char value[4];
		_itoa_s(hex.value, value, 4, 10);
		al_draw_filled_rounded_rectangle(x + MAP_CELL_SIZE / 4, y + MAP_CELL_SIZE / 2 - MAP_CELL_SIZE / 8, x + MAP_CELL_SIZE * 3 / 4, y + MAP_CELL_SIZE / 2 + MAP_CELL_SIZE / 8, 8, 8, al_map_rgb(255, 255, 255));
		al_draw_text(font, al_map_rgb(0, 0, 0), x + MAP_CELL_SIZE / 2, y + MAP_CELL_SIZE / 2 - al_get_font_line_height(font) / 2, ALLEGRO_ALIGN_CENTER, value);
	
		if (overlay) {
			for (int i = 0; i < 6; i++) {
				int next = (i + 1) % 6;
				al_draw_filled_triangle(x + MAP_CELL_SIZE / 2, y + MAP_CELL_SIZE / 2, vertices[i * 2], vertices[i * 2 + 1], vertices[next * 2], vertices[next * 2 + 1], al_map_rgba(0, 0, 0, 128));
			}
		}
	}

	// draw road (outline)
	al_draw_polygon(vertices, 6, ALLEGRO_LINE_JOIN_BEVEL, ResourceColor[DESERT], MAP_ROAD_THICKNESS, 0.0);
	//al_draw_polygon(vertices, 6, ALLEGRO_LINE_JOIN_BEVEL, al_map_rgb(225, 181, 104), MAP_ROAD_THICKNESS, 0.0);
}

void draw_map(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, int dices)
{
	al_clear_to_color(ResourceColor[WATER]);

	// draw tiles
	for (int pos = 0; pos < MAX_TILES - 1; ++pos) {
		draw_hex(board.tiles[pos], font, dices == board.tiles[pos].value);
		// [DEBIG] SHOW TILE INDEXES
		//#ifdef _DEBUG_SHOW_GRIDS)
		//	char str[4];
		//	_itoa_s(pos, str, 4, 10);
		//	al_draw_text(font, al_map_rgb(255, 0, 0), board.tiles[pos].x + grid_offset.x, board.tiles[pos].y + grid_offset.y, 0, str);
		//#endif
	}

	// [DEBUG] SHOW SQUARE BOUNDING BOXES
	#ifdef _DEBUG_SHOW_GRIDS
	for (int i = 0; i < MAX_TILES; ++i)
		draw_bounding_box(board.tiles[i]);
	#endif
}

void draw_vertices(ALLEGRO_DISPLAY* display, ALLEGRO_FONT* font, int selected, ALLEGRO_COLOR player_color) {
	// [DEBUG] SHOW ALL VERTICES
	for (int i = 0; i < MAX_VERTICES; ++i) {
		PLACEMENT v = board.placements[i];
		if (v.active == false) continue;
		ALLEGRO_COLOR color = player_color;
		if (selected != -1) {
			if (i == selected) color = al_map_rgb(255, 255, 255);
			else if (i == board.placements[selected].neighbors[0] || i == board.placements[selected].neighbors[1] || i == board.placements[selected].neighbors[2]) {
				color = al_map_rgb((255 + player_color.r * 255) * 0.5, (255 + player_color.g * 255) * 0.5, (255 + player_color.b * 255) * 0.5);
			}
		}
		al_draw_filled_circle(v.x + grid_offset.x, v.y + grid_offset.y, 8, color);
	}
}

#ifdef _DEBUG_SHOW_GRIDS
	void draw_bounding_box(HEX hex) {
		if (hex.type == WATER) return;

		float x = hex.x + grid_offset.x;
		float y = hex.y + grid_offset.y;
		al_draw_rectangle(x, y, x + MAP_CELL_SIZE, y + MAP_CELL_SIZE, al_map_rgb(255, 0, 0), 1.0);
	}
#endif

#ifdef _DEBUG_GRID_EXTRA_OUT
	void print_vertex_data(int pos, char* title, int p0, int p1, int p2, int n0, int n1, int n2) {
		printf("v[%d] (%s):\n", pos, title);
		printf("   parents - %d, %d, %d\n", p0, p1, p2);
		printf("   neighbors - %d, %d, %d\n", n0, n1, n2);
	}
#endif
