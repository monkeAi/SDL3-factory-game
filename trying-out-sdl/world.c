#include "world.h"  
#include "constants.h"  
#include "SDL3/SDL.h"
#include "perlin_noise.h"
#include "camera.h"
#include "tools.h"
<<<<<<< HEAD
#include "ore_manager.h"

=======
#include "OreManager.h"
>>>>>>> fb4f3e309b1b3716e62547e19f4a3090296cade4


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

	// create ore patches

	struct Ore_Manager* ore_Manager = get_Ore_Manager();



	for (int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {

			GAME_SEED = SEED + 1;
			float perlin_Iron = perlin2d(x, y, NOISE_FREQ, NOISE_AMP) * 100.0;

			GAME_SEED = SEED + 2;
			float perlin_Copper = perlin2d(x, y, NOISE_FREQ, NOISE_AMP) * 100.0;

			GAME_SEED = SEED + 3;
			float perlin_Coal = perlin2d(x, y, NOISE_FREQ, NOISE_AMP) * 100.0;

			enum Ore_Type ore_Type;
			int ore_Amount = 0;
			bool has_Ore = false;

			if (perlin_Iron < IRON_ORE_RANGE) {
				ore_Type = IRON;
				ore_Amount = perlin_Iron * 1000;
				has_Ore = true;
			}
			if (perlin_Copper < COPPER_ORE_RANGE) {
				ore_Type = COPPER;
				ore_Amount = perlin_Copper * 1000;
				has_Ore = true;
			}
			if (perlin_Coal < COAL_ORE_RANGE) {
				ore_Type = COAL;
				ore_Amount = perlin_Coal * 1000;
				has_Ore = true;
			}
			if (has_Ore && map[y][x].type != TILE_WATER) {
				ore_Manager->add_Ore(ore_Manager, x, y, ore_Type, ore_Amount);
			}

		}
	}

	//print_tilemap();

	// create ore patches

	struct OreManager* oreManager = getOreManager();

	

	for (int y = 0; y < MAP_HEIGHT; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {

			GAME_SEED = SEED + 1;
			float perlinIron = perlin2d(x, y, NOISE_FREQ, NOISE_AMP) * 100.0;

			GAME_SEED = SEED + 2;
			float perlinCopper = perlin2d(x, y, NOISE_FREQ, NOISE_AMP) * 100.0;

			GAME_SEED = SEED + 3;
			float perlinCoal = perlin2d(x, y, NOISE_FREQ, NOISE_AMP) * 100.0;

			enum OreType oreType;
			int oreAmount = 0;
			bool hasOre = false;

			if (perlinIron < IRON_ORE_RANGE) {
				oreType = IRON;
				oreAmount = perlinIron * 1000;
				hasOre = true;
			}
			if (perlinCopper < COPPER_ORE_RANGE) {
				oreType = COPPER;
				oreAmount = perlinCopper * 1000;
				hasOre = true;
			}
			if (perlinCoal < COAL_ORE_RANGE) {
				oreType = COAL;
				oreAmount = perlinCoal * 1000;
				hasOre = true;
			}
			if (hasOre && map[y][x].type != TILE_WATER) {
				oreManager->addOre(oreManager, x, y, oreType, oreAmount);
			}
			
		}
	}
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


			float screenRatioW = WINDOW_WIDTH / mainCamera->width;
			float screenRatioH = WINDOW_HEIGHT / mainCamera->height;

			SDL_FRect tile_rect = {
				(x * TILE_SIZE - mainCamera->x_offset) * screenRatioW,
				(y * TILE_SIZE - mainCamera->y_offset) * screenRatioH,
				TILE_SIZE * screenRatioW + 1,
				TILE_SIZE * screenRatioH + 1
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
			//case TILE_SELECTED: rgba[0] = 0; rgba[1] = 0; rgba[2] = 0; break;
			case TILE_FULL: rgba[0] = 100; rgba[1] = 0; rgba[2] = 0; break;
			}

			SDL_SetRenderDrawColor(renderer, rgba[0], rgba[1], rgba[2], rgba[3]);
			
			// Add to render 
			SDL_RenderFillRect(renderer, &tile_rect);
		}
	}

	return 0;
}

int render_ores(SDL_Renderer* renderer) {
<<<<<<< HEAD
	struct Ore_Manager* ore_Manager = get_Ore_Manager();
	struct Ore_List* curr = ore_Manager->ore_List;

	while (curr != NULL) {
		struct Ore* ore = &curr->ore;

		SDL_FRect tile_rect = {
				ore->x * TILE_SIZE + world_start_offset_x - mainCamera->x_offset,
				ore->y * TILE_SIZE + world_start_offset_y - mainCamera->y_offset,
				TILE_SIZE,
				TILE_SIZE
		};

		// Depending on tile type select corresponding tile color
		static Uint8 rgba[4] = { 255 };

		// In future select the right texture
		switch (ore->type) {
			case IRON: rgba[0] = 50; rgba[1] = 50; rgba[2] = 200; rgba[3] = 50; break;
			case COPPER: rgba[0] = 200; rgba[1] = 100; rgba[2] = 100; rgba[3] = 50; break;
			case COAL: rgba[0] = 50; rgba[1] = 50; rgba[2] = 50; rgba[3] = 50; break;
			default: rgba[0] = 255; rgba[1] = 255; rgba[2] = 255; rgba[3] = 50; break;
		}

		SDL_SetRenderDrawColor(renderer, rgba[0], rgba[1], rgba[2], rgba[3]);

		// Add to render 
		SDL_RenderFillRect(renderer, &tile_rect);

		curr = curr->next;
	}
}

// Transform world cordinates to table index of tile
void cordinate_to_index(int *cordinates, int *tileIndex) {
=======
	struct OreManager* oreManager = getOreManager();
	struct OreList* curr = oreManager->oreList;
>>>>>>> fb4f3e309b1b3716e62547e19f4a3090296cade4

	while (curr != NULL) {
		struct Ore* ore = &curr->ore;
		static Uint8 rgba[4] = { 255 };

		float screenRatioW = WINDOW_WIDTH / mainCamera->width;
		float screenRatioH = WINDOW_HEIGHT / mainCamera->height;

		SDL_FRect tile_rect = {
			(ore->x * TILE_SIZE - mainCamera->x_offset) * screenRatioW,
			(ore->y * TILE_SIZE - mainCamera->y_offset) * screenRatioH,
			TILE_SIZE * screenRatioW + 1,
			TILE_SIZE * screenRatioH + 1
		};

		// In future select the right texture
		switch (ore->type) {
			case IRON: rgba[0] = 50; rgba[1] = 50; rgba[2] = 200; rgba[3] = 50; break;
			case COPPER: rgba[0] = 200; rgba[1] = 100; rgba[2] = 100; rgba[3] = 50; break;
			case COAL: rgba[0] = 50; rgba[1] = 50; rgba[2] = 50; rgba[3] = 50; break;
			default: rgba[0] = 255; rgba[1] = 255; rgba[2] = 255; rgba[3] = 50; break;
		}

		SDL_SetRenderDrawColor(renderer, rgba[0], rgba[1], rgba[2], rgba[3]);

		// Add to render 
		SDL_RenderFillRect(renderer, &tile_rect);

		curr = curr->next;
	}
}

// Transform world cordinates to table index of tile
void cordinate_to_tile(int *cordinates, int *tileIndex) {

	tileIndex[0] = floor(cordinates[0] / TILE_SIZE);	//	X cordinate 
	tileIndex[1] = floor(cordinates[1] / TILE_SIZE);	//	Y cordinate

	//return 0;
}

// Returns true if index coordinates are inside the world map
int world_is_inside(int x, int y) {
	if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) return TRUE;
	else return FALSE;
}
