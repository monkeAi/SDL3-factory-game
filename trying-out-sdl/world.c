#include "world.h"  
#include "constants.h"  
#include "SDL3/SDL.h"
#include "perlin_noise.h"

struct GameTile {
	int xIndex;
	int yIndex;
	enum TileType type;
	enum TileState state;
	//texture pointer

};

// Create 2D map made of different tiles
struct GameTile map[MAP_HEIGHT][MAP_WIDTH];	// [y][x]

// Init world tilemap
void tilemap_init() {

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

// Render 
int tilemap_render(SDL_Renderer* renderer) {

	// Loop through entire map
	for (int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {

			// Create a rect for selected tile
			SDL_FRect tile_rect = {
				x * TILE_SIZE,
				y * TILE_SIZE,
				TILE_SIZE,
				TILE_SIZE
			};

			// Depending on tile type select corresponding tile color
			// In future select the right texture
			switch (map[y][x].type) {
				case TILE_CONCRETE: SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); break;
				case TILE_EMPTY: SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); break;
				case TILE_GRASS_1: SDL_SetRenderDrawColor(renderer, 143, 207, 90, 255); break;
				case TILE_GRASS_2: SDL_SetRenderDrawColor(renderer, 100, 176, 104, 255); break;
				case TILE_SAND: SDL_SetRenderDrawColor(renderer, 252, 246, 164, 255); break;
				case TILE_WATER: SDL_SetRenderDrawColor(renderer, 66, 210, 255, 255); break;
				case TILE_ROCKY: SDL_SetRenderDrawColor(renderer, 191, 188, 159, 255); break;
			}
			
			// Add to render 
			SDL_RenderFillRect(renderer, &tile_rect);
		}
	}

	return 0;
}