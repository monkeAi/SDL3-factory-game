#include "world.h"  
#include "constants.h"  
#include "SDL3/SDL.h"
#include "perlin_noise.h"
#include "camera.h"
#include "tools.h"



// Create 2D map made of different tiles
struct GameTile map[MAP_HEIGHT][MAP_WIDTH];	// [y][x]

// Calculate world offset from window to tile border in px

float world_start_offset_x = (WINDOW_WIDTH / 2) - (TILE_SIZE * MAP_WIDTH / 2);
float world_start_offset_y = (WINDOW_HEIGHT / 2) - (TILE_SIZE * MAP_HEIGHT / 2);

// Calculate world origin to translate 0,0 point to center of map
int world_origin_x = (WINDOW_WIDTH / 2);
int world_origin_y = (WINDOW_HEIGHT / 2);

// Tile offset
int world_map_center_x = MAP_WIDTH / 2;
int world_map_center_y = MAP_HEIGHT / 2;

// Init world tilemap
void init_tilemap() {

	GAME_SEED = SEED;
	
	// Loop throught every spot and create a tile
	for (int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {

			// Asign struct parameters
			map[y][x].xIndex = x;
			map[y][x].yIndex = y;
			map[y][x].state = TILE_FREE;

			// Get perlin noise value and map it to a corresponding enviroment type
			float perlinNoiseValue = perlin2d(x, y, NOISE_FREQ, NOISE_AMP) * 100.0;

			//printf("%f %d %d\n", perlinNoiseValue, x, y);
			enum TileType selectedType;

			// Assign different Tile types based on noise range
			if (perlinNoiseValue < TERRAIN_WATER_RANGE) { selectedType = TILE_WATER; }
			else if (perlinNoiseValue < TERRAIN_SAND_RANGE) { selectedType = TILE_SAND; }
			else if (perlinNoiseValue < TERRAIN_GRASS_RANGE) { selectedType = TILE_GRASS_1; }
			else if (perlinNoiseValue < TERRAIN_FORREST_RANGE) { selectedType = TILE_GRASS_2; }
			else if (perlinNoiseValue < TERRAIN_ROCKY_RANGE) { selectedType = TILE_ROCKY; }
			else {
				printf("Error: perlin noise value out of bounds.\n");
				selectedType = TILE_EMPTY;
			}

			map[y][x].type = selectedType;
		}
	}

	//print_tilemap();

}

// Converts enums to characters
char tiletype_to_char(enum TileType t) {
	switch (t) {
	case TILE_CONCRETE: return '#';
	case TILE_GRASS_1: return '.';
	case TILE_EMPTY: return ' ';
	default: return '?';
	}
}

// Prints out the the wntire map in console
static void print_tilemap() {
	for (int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {
			putchar(tiletype_to_char(map[y][x].type));
		}
		putchar('\n');
	}
}

int update_tilemap() {
	// Loop through entire map
	for (int y = 0; y < MAP_HEIGHT; y++) {

		for (int x = 0; x < MAP_WIDTH; x++) {

			// Deselect tile
			if (map[y][x].state == TILE_SELECTED) map[y][x].state = TILE_FREE;
		}
	}


	return 0;
}

// Render 
int render_tilemap(SDL_Renderer* renderer) {

	// Loop through entire map
	for (int y = 0; y < MAP_HEIGHT; y++) {

		for (int x = 0; x < MAP_WIDTH; x++) {

			// Create a rect for selected tile
			// Positions are calculated based on origin and player
			SDL_FRect tile_rect = {
				x * TILE_SIZE + world_start_offset_x - mainCamera->x_offset,
				y * TILE_SIZE + world_start_offset_y - mainCamera->y_offset,
				TILE_SIZE,
				TILE_SIZE
			};

			// Depending on tile type select corresponding tile color
			static Uint8 rgba[4] = { 255 };


			// In future select the right texture
			switch (map[y][x].type) {
				case TILE_CONCRETE: rgba[0] = 100; rgba[1] = 100; rgba[2] = 100; rgba[3] = 255; break;
				case TILE_EMPTY: rgba[0] = 0; rgba[1] = 0; rgba[2] = 0; rgba[3] = 255; break;
				case TILE_GRASS_1: rgba[0] = 143; rgba[1] = 207; rgba[2] = 90; rgba[3] = 255; break;
				case TILE_GRASS_2: rgba[0] = 100; rgba[1] = 176; rgba[2] = 104; rgba[3] = 255; break;
				case TILE_SAND: rgba[0] = 252; rgba[1] = 246; rgba[2] = 164; rgba[3] = 255; break;
				case TILE_WATER: rgba[0] = 66; rgba[1] = 210; rgba[2] = 255; rgba[3] = 255; break;
				case TILE_ROCKY: rgba[0] = 191; rgba[1] = 188; rgba[2] = 159; rgba[3] = 255; break;
			}

			switch (map[y][x].state) {
			case TILE_SELECTED: rgba[0] = 0; rgba[1] = 0; rgba[2] = 0; break;
			case TILE_FULL: rgba[0] = 100; rgba[1] = 0; rgba[2] = 0; break;
			}

			SDL_SetRenderDrawColor(renderer, rgba[0], rgba[1], rgba[2], rgba[3]);
			
			// Add to render 
			SDL_RenderFillRect(renderer, &tile_rect);
		}
	}

	return 0;
}

// Transform world cordinates to table index of tile
void cordinate_to_index(int *cordinates, int *tileIndex) {

	tileIndex[0] = world_map_center_x + cordinates[0];	//	X cordinate 
	tileIndex[1] = world_map_center_y - cordinates[1] - 1;	//	Y cordinate

	//return 0;
}
