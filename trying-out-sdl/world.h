#ifndef WORLD_H
#define WORLD_H

#include <SDL3/SDL.h>

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

void tilemap_init();
int tilemap_render(SDL_Renderer* renderer);
void print_tilemap();

#endif