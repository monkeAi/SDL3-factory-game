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
	enum ItemType building_item_type;

	struct Inventory* input_inv;
	struct Inventory* output_inv;
	int input_size, output_size;

	enum RecipeName recipe;
	int craft_request_id;

	unsigned int color;
	float work_time_left;
};

enum BuildingType {
	BUILDING_NONE,
	BUILDING_BURNER_MINER,
	BUILDING_ELECTRIC_MINER,
	BUILDING_BURNER_SMELTER,
	BUILDING_CRAFTER_1,
	BUILDING_INSERTER,
	BUILDING_INSERTER_LONG,
	BUILDING_CONVEYOR
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

void update_buildings(float delta_time);
void render_buildings(SDL_Renderer* renderer);

// Global buildings list

extern struct Building* Buildings[MAX_BUILDINGS];

#endif
