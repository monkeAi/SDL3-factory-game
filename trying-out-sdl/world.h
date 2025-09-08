#ifndef WORLD_H
#define WORLD_H

#include <SDL3/SDL.h>
#include "tools.h"
#include "constants.h"

// All different tile types
enum TileType {
	TILE_CONCRETE,
	TILE_EMPTY,
	TILE_GRASS_1,
	TILE_GRASS_2,
	TILE_ROCKY,
	TILE_SAND,
	TILE_WATER
};

enum TileState {
	TILE_FREE,
	TILE_FULL,
	TILE_SELECTED
};

struct GameTile {
	int xIndex;
	int yIndex;
	enum TileType type;
	enum TileState state;
	//texture pointer
};

extern struct GameTile map[MAP_HEIGHT][MAP_WIDTH];

float world_start_offset_x;
float world_start_offset_y;
int world_origin_x;
int world_origin_y;

void init_tilemap();
int render_tilemap(SDL_Renderer* renderer);
void print_tilemap();
void cordinate_to_index(int* cordinates, int* tileIndex);
int world_is_inside(int x, int y);

int render_ores(SDL_Renderer* renderer);

#endif