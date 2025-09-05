#ifndef BUILDINGS_H
#define BUILDINGS_H

#include "inventory.h"
#include "recipes.h"
#include "gui.h"

struct Building {
	enum BuildingType type;
	struct Vector2D* coords;
	enum BuildingRotation rotation;
	int x_offset, y_offset;
	enum BuildingState state;
	int tile_width, tile_height;

	struct Inventory* input_inv;
	struct Inventory* output_inv;

	int input_size, output_size;
	enum RecipeName recipe;
};

enum BuildingType {
	BUILDING_NONE,
	BUILDING_BURNER_MINER,
	BUILDING_ELECTRIC_MINER,
	BUILDING_BURNER_SMELTER,
	BUILDING_CRAFTER_1,
};

enum BuildingState {
	BUILDING_STATE_NONE,
	BUILDING_STATE_IDLE,
	BUILDING_STATE_RUNNING,
	BUILDING_STATE_PAUSED
};

enum BuildingRotation {
	DOWN,
	LEFT,
	UP,
	RIGHT
};

// Functions
int Building_create(enum BuildingType type, int* coordinates, enum BuildingRotation rotation);
void Building_destroy(struct Building* building);
void Building_rotate(struct Building* b, enum BuildingRotation rotation);
void Buildings_print();
void Building_free(struct Building* building);
int Building_is_inside(struct Building* b, int x_coord, int y_coord);

// Global buildings list

extern struct Building* Buildings[MAX_BUILDINGS];

#endif
