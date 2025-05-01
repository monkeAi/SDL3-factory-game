#ifndef WORLD_H
#define WORLD_H

#include <SDL3/SDL.h>
#include "tools.h"

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

int world_offset_x;
int world_offset_y;
int world_origin_x;
int world_origin_y;

void init_tilemap();
int render_tilemap(SDL_Renderer* renderer);
void print_tilemap();

#endif