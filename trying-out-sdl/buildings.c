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

	// Check if there is space for a building
	int buildings_slot = Building_find_free_slot();
	if (buildings_slot == -1) return 1;

	// Check if space is available at the cordinates for the selected building with rotation applied
	if (!Building_placement_available(type, coordinates, rotation)) return 1;

	// Init a new building
	struct Building* b = Building_init(type);
	b->rotation = rotation;

	Building_rotate(b, rotation);
	b->coords->x = coordinates[0] - b->x_offset;
	b->coords->y = coordinates[1] - b->y_offset;		// FIX HERE

	//printf("Building coordinates x: %d, y: %d\n", (int)b->coords->x, (int)b->coords->y);

	// Asign new tile state to occupied tiles
	int selected_tile_index[2];
	int selected_cords[2] = { b->coords->x, b->coords->y };
	cordinate_to_index(selected_cords, selected_tile_index);

	// Asign tile_full state to all occupied tiles
	for (int y = b->coords->y; y <= b->coords->y + b->tile_height - 1; y++) {
		for (int x = b->coords->x; x <= (b->coords->x + b->tile_width - 1); x++) {

			// Convert coordinates to tile index
			int selected_tile_index[2];
			int selected_cords[2] = { x, y };
			cordinate_to_index(selected_cords, selected_tile_index);

			// Set map tiles to full
			map[selected_tile_index[1]][selected_tile_index[0]].state = TILE_FULL;

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
void Building_rotate(struct Building* b, enum BuildingRotation rotation) {

	int building_width = b->tile_width;
	int building_height = b->tile_height;

	// Apply rotation
	// Left and right
	if (rotation % 2 == 1) {
		int temp = building_width;
		building_width = building_height;
		building_height = temp;
	}

	b->tile_width = building_width;
	b->tile_height = building_height;

	// Calculate new offsets

	if (building_width % 2 == 1) b->x_offset = (building_width - 1) / 2;	// Odd numbers
	else b->x_offset = (building_width / 2) - 1;							// Even numbers

	if (building_height % 2 == 1) b->y_offset = (building_height - 1) / 2;	// Odd numbers
	else b->y_offset = (building_height / 2) - 1;							// Even numbers

	//printf("Building offset x: %d, y: %d\n", b->x_offset, b->y_offset);

	return;
}


// Destroys the building and stores the contents in temporary inventory
void Building_destroy(struct Building* b) {

	// Free up all occupied tiles
	for (int y = b->coords->y; y <= b->coords->y + b->tile_height - 1; y++) {
		for (int x = b->coords->x; x <= (b->coords->x + b->tile_width - 1); x++) {

			// Convert coordinates to tile index
			int selected_tile_index[2];
			int selected_cords[2] = { x, y };
			cordinate_to_index(selected_cords, selected_tile_index);

			// Set map tiles to full
			map[selected_tile_index[1]][selected_tile_index[0]].state = TILE_FREE;

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

	// Apply rotation
	// Left and right
	if (rotation % 2 == 1) {
		int temp = building_width;
		building_width = building_height;
		building_height = temp;
	}

	// Check with world tiles
	int x_offset, y_offset;

	if (building_width % 2 == 1) x_offset = (building_width - 1) / 2;	// Odd numbers
	else x_offset = (building_width / 2) - 1;							// Even numbers

	if (building_height % 2 == 1) y_offset = (building_height - 1) / 2;	// Odd numbers
	else y_offset = (building_height / 2) - 1;							// Even numbers

	// Loop through height and width
	for (int y = coordinates[1] - y_offset; y <= coordinates[1] - y_offset + building_height - 1; y++) {

		for (int x = coordinates[0] - x_offset; x <= (coordinates[0] - x_offset + building_width - 1); x++) {

			int selected_tile_index[2];
			int selected_cords[2] = { x, y };

			cordinate_to_index(selected_cords, selected_tile_index);

			// Check if tile is out of bounds
			if (!(selected_tile_index[0] >= 0 && selected_tile_index[0] < MAP_WIDTH && selected_tile_index[1] >= 0 && selected_tile_index[1] < MAP_HEIGHT)) {
				return 0;
			}

			// Check if tile is free and not water
			if (map[selected_tile_index[1]][selected_tile_index[0]].state == TILE_FULL || map[selected_tile_index[1]][selected_tile_index[0]].type == TILE_WATER) {
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
	for (slot = 0; Buildings[slot] != NULL && slot < MAX_BUILDINGS; slot++);
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

		SDL_FRect building_rect = {
			b->coords->x * TILE_SIZE + world_origin_x - mainCamera->x_offset,
			b->coords->y * TILE_SIZE * -1 - b->tile_height * TILE_SIZE + world_origin_y - mainCamera->y_offset,
			TILE_SIZE * b->tile_width,
			TILE_SIZE * b->tile_height
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