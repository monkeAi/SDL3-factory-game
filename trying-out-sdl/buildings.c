#include <stdio.h>
#include <stdlib.h>
#include "SDL3/SDL.h"

#include "buildings.h"
#include "tools.h"
#include "constants.h"
#include "world.h"
#include "inventory.h"
#include "camera.h"

// List for all building's pointers
struct Building* Buildings[MAX_BUILDINGS] = { NULL };

// Initialize new building struct
static struct Building* Building_init(enum BuildingType type) {

	// Allocate enough memory to the building pointer
	struct Building* building = (struct Building*)malloc(sizeof(struct Building));
	if (building == NULL) {
		fprintf(stderr, "Failed to allocate memory for building.\n");
		exit(1);
	}

	building->type = type;
	building->recipe = RECIPE_NONE;
	building->state = BUILDING_NONE;
	building->input_size = 0;
	building->output_size = 0;
	building->coords = malloc(sizeof(struct Vector2D));

	// Depending on the building type set the parameters
	switch (type) {
	case BUILDING_NONE:
		building->input_inv = NULL;
		building->output_inv = NULL;
		building->tile_width = 0;
		building->tile_height = 0;
		break;

	case BUILDING_CRAFTER_1:
	    building->input_inv = Inventory_create(CRAFTER_MAX_INV_SLOTS, 0);
		building->output_inv = Inventory_create(CRAFTER_MAX_INV_SLOTS, 0);
		building->tile_width = CRAFTER_WIDTH;
		building->tile_height = CRAFTER_HEIGHT;
		break;

	default:
		building->input_inv = NULL;
		building->output_inv = NULL;
		building->tile_width = 0;
		building->tile_height = 0;
		break;
	}

	return building;
}


// Create and place a new building into the world
int Building_create(enum BuildingType type, int* coordinates, enum BuildingRotation rotation) {

	//printf("Trying to create building of type: %d at coordinates x: %d, y: %d with rotation: %d\n", type, coordinates[0], coordinates[1], rotation);
	// Check if there is space for a building
	int buildings_slot = Building_find_free_slot();
	if (buildings_slot == -1) return 1;

	//printf("Creating building at slot: %d\n", buildings_slot);
	// Check if space is available at the cordinates for the selected building with rotation applied
	if (!Building_placement_available(type, coordinates, rotation)) return 1;

	// Init a new building
	struct Building* b = Building_init(type);
	b->rotation = rotation;
	b->coords->x = coordinates[0];
	b->coords->y = coordinates[1];		// FIX HERE

	//printf("Building coordinates x: %d, y: %d\n", (int)b->coords->x, (int)b->coords->y);

	// Asign new tile state to occupied tiles
	int selected_tile_index[2] = { b->coords->x, b->coords->y };

	// Asign tile_full state to all occupied tiles
	for (int y = b->coords->y; y <= b->coords->y + b->tile_height - 1; y++) {
		for (int x = b->coords->x; x <= (b->coords->x + b->tile_width - 1); x++) {
			
			map[y][x].state = TILE_FULL;

		}
	}

	// Add pointer of building to a list of all buildings
	Buildings[buildings_slot] = b;

	//printf("Building created. \n");

	// Return 0 SUCESS
	// Return 1 Invalid Space
	return 0;
}


// Rotates the building struct to specified rotation
//nerabis neke ful kode za rotiranje ker so vse zgradbe kvadrati trenutno
//je samo vizualno kako se tekstura obrne in funkcionalnost rotacije lahko pol dobis vn is b->rotation

void Building_rotate_once(struct Building* b, bool direction) {
	if (direction == 0) {
		b->rotation = (b->rotation + 1) % 4;
	} else if (direction == 1) {
		b->rotation = (b->rotation - 1 + 4) % 4;
	}
	return;
}

void Building_rotate(struct Building* b, enum BuildingRotation rotation) {
	b->rotation = rotation;
	return;
}


// Destroys the building and stores the contents in temporary inventory
void Building_destroy(struct Building* b) {

	// Free up all occupied tiles
	for (int y = b->coords->y; y <= b->coords->y + b->tile_height - 1; y++) {
		for (int x = b->coords->x; x <= (b->coords->x + b->tile_width - 1); x++) {

			int selected_tile[2] = { x, y };
			map[selected_tile[0]][selected_tile[1]].state = TILE_FREE;

		}
	}

	// Find the building in the Buildings list and remove it;
	int slot;
	for (slot = 0; Buildings[slot] != b && slot < MAX_BUILDINGS; slot++);
	if (slot == MAX_BUILDINGS) return 1;
	Buildings[slot] = NULL;

	// Create the building item and give it to player
		// Handle case when player inventory is full


	// Free allocated memory 
	Building_free(b);
}

// Return true if building can be placed in that spot
static int Building_placement_available(enum BuildingType type, int* coordinates, enum BuildingRotation rotation) {
	
	int building_width = 0, building_height = 0;

	// Get the size of the building
	switch (type) {
	case BUILDING_NONE:
		break;

	case BUILDING_CRAFTER_1:
		building_width = CRAFTER_WIDTH;
		building_height = CRAFTER_HEIGHT;
	}

	// Loop through height and width
	for (int x = coordinates[0]; x <= coordinates[0] + building_height - 1; x++) {

		for (int y = coordinates[1]; y <= (coordinates[1] + building_width - 1); y++) {

			if (!(x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT)) {
				return 0;
			}

			// Check if tile is free and not water
			if (map[x][y].state == TILE_FULL || map[x][y].type == TILE_WATER) {
				return 0;
			}

		}
	}

	// Return if space is available
	return 1;	
}


// Return first free slot in the buildings list, returns -1 if list is full
static int Building_find_free_slot() {
	int slot;
	for (slot = 0; Buildings[slot] != NULL && slot < MAX_BUILDINGS; slot++) {}
	if (slot == MAX_BUILDINGS) return -1;
	return slot;
}


// Loops through all the buildings and updates them
void update_buildings() {


}


// Loops through all the buildings and renders them
void render_buildings(SDL_Renderer* renderer) {

	for (int i = 0; i < MAX_BUILDINGS; i++) {

		struct Building* b = Buildings[i];

		// Skip if null
		if (b == NULL) continue;	

		//printf("x: %d, y: %d\n", b->x_offset, b->y_offset);

		float screenRatioW = WINDOW_WIDTH / mainCamera->width;
		float screenRatioH = WINDOW_HEIGHT / mainCamera->height;

		SDL_FRect building_rect = {
			(b->coords->x * TILE_SIZE - mainCamera->x_offset) * screenRatioW,
			(b->coords->y * TILE_SIZE - mainCamera->y_offset) * screenRatioH,
			TILE_SIZE * b->tile_width * screenRatioW + 1,
			TILE_SIZE * b->tile_height * screenRatioH + 1
		};

		// Correct the offset and account for rotation
		// Set render color
		SDL_SetRenderDrawColor(renderer, 255, 189, 51, 255);

		// Add to render  
		SDL_RenderFillRect(renderer, &building_rect);
	}

}


// Prints out the buildings list in terminal
void Buildings_print() {
	for (unsigned int i = 0; i < MAX_BUILDINGS; i++) {
		if (Buildings[i] != NULL) {
			printf("Slot %d: Type: %d, Rotation: %d, Coords: (%f, %f), Recipe: %d\n", i, Buildings[i]->type, Buildings[i]->rotation, Buildings[i]->coords->x, Buildings[i]->coords->y, Buildings[i]->recipe);
		}
		else {
			printf("Slot %d: NULL\n", i);
		}
	}
}


// Return true if cursor coordinate is inside building
int Building_is_inside(struct Building* b, int x_coord, int y_coord) {

	if (x_coord >= b->coords->x && x_coord <= b->coords->x + b->tile_width && y_coord >= b->coords->y && y_coord <= b->coords->y + b->tile_height) return TRUE;
	else return FALSE;

}


// Frees memory of building struct and its children
static void Building_free(struct Building* building) {

	if (building == NULL) return;

	if (building->coords != NULL) {
		free(building->coords);
		building->coords == NULL;
	}

	if (building->input_inv != NULL) {
		Inventory_free(building->input_inv);
	}

	if (building->output_inv != NULL) {
		Inventory_free(building->output_inv);
	}

	free(building);
}