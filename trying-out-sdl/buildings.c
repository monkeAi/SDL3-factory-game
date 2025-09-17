#include <stdio.h>
#include <stdlib.h>
#include "SDL3/SDL.h"
#include "SDL3/SDL_stdinc.h"

#include "buildings.h"
#include "tools.h"
#include "constants.h"
#include "world.h"
#include "inventory.h"
#include "camera.h"
#include "crafting.h"
#include "recipes.h"
#include "OreManager.h"

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
	building->state = BUILDING_STATE_IDLE;
	building->input_size = 0;
	building->output_size = 0;
	building->coords = malloc(sizeof(struct Vector2D));
	building->craft_request_id = NULL;
	building->work_time_left = 0;

	// Depending on the building type set the parameters
	switch (type) {
	case BUILDING_NONE:
		building->input_inv = NULL;
		building->output_inv = NULL;
		building->tile_width = 0;
		building->tile_height = 0;
		building->building_item_type = ITEM_NONE;
		building->color = 0x000000ff;
		break;

	case BUILDING_CRAFTER_1:
	    building->input_inv = Inventory_create(CRAFTER_MAX_INPUT, CRAFTER_MAX_INPUT);
		building->output_inv = Inventory_create(CRAFTER_MAX_OUTPUT, CRAFTER_MAX_OUTPUT);
		building->tile_width = CRAFTER_WIDTH;
		building->tile_height = CRAFTER_HEIGHT;
		building->building_item_type = ITEM_CRAFTER_1;
		building->color = 0xFFBD33FF;
		break;

	case BUILDING_BURNER_MINER:
		building->input_inv = Inventory_create(CRAFTER_MAX_INPUT, CRAFTER_MAX_INPUT);
		building->output_inv = Inventory_create(CRAFTER_MAX_INPUT, CRAFTER_MAX_INPUT);
		building->tile_width = MINER_WIDTH;
		building->tile_height = MINER_HEIGHT;
		building->building_item_type = ITEM_BURNER_MINER;
		building->color = 0xa84e32ff;
		break;

	case BUILDING_BURNER_SMELTER:
		building->input_inv = Inventory_create(CRAFTER_MAX_INPUT, CRAFTER_MAX_INPUT);
		building->output_inv = Inventory_create(CRAFTER_MAX_OUTPUT, CRAFTER_MAX_OUTPUT);
		building->tile_width = SMELTER_WIDTH;
		building->tile_height = SMELTER_HEIGHT;
		building->building_item_type = ITEM_BURNER_SMELTER;
		building->color = 0xedda5fff;
		break;

	case BUILDING_INSERTER:
		building->input_inv = Inventory_create(1, 1);
		building->output_inv = Inventory_create(1, 1);
		building->tile_width = 1;
		building->tile_height = 1;
		building->building_item_type = ITEM_INSERTER;
		building->color = 0x325e8aff;
		building->work_time_left = INSERTER_WORK_TIME;
		break;

	case BUILDING_CONVEYOR:
		building->input_inv = Inventory_create(1, 1);
		building->output_inv = Inventory_create(1, 1);
		building->tile_width = 1;
		building->tile_height = 1;
		building->building_item_type = ITEM_CONVEYOR;
		building->color = 0xffb60aff;
		building->work_time_left = CONVEYOR_WORK_TIME;
		break;
		
	case BUILDING_INSERTER_LONG:
		building->input_inv = Inventory_create(1, 1);
		building->output_inv = Inventory_create(1, 1);
		building->tile_width = 1;
		building->tile_height = 1;
		building->building_item_type = ITEM_INSERTER_LONG;
		building->color = 0xc4063cff;
		building->work_time_left = INSERTER_WORK_TIME;
		break;

	default:
		building->input_inv = NULL;
		building->output_inv = NULL;
		building->tile_width = 1;
		building->tile_height = 1;
		building->building_item_type = ITEM_NONE;
		building->color = 0x000000ff;
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

	// Cancel craft request
	CraftingQueue[b->craft_request_id].output_inv = NULL;


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
		break;

	case BUILDING_BURNER_MINER:
		building_width = MINER_WIDTH;
		building_height = MINER_HEIGHT;
		break;
	case BUILDING_BURNER_SMELTER:
		building_width = SMELTER_WIDTH;
		building_height = SMELTER_HEIGHT;
		break;
	case BUILDING_INSERTER:
		building_width = 1;
		building_height = 1;
		break;
	case BUILDING_INSERTER_LONG:
		building_width = 1;
		building_height = 1;
		break;
	case BUILDING_CONVEYOR:
		building_width = 1;
		building_height = 1;
		break;

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
void update_buildings(float delta_time) {

	for (int b = 0; b < MAX_BUILDINGS; b++) {

		if (Buildings[b] == NULL) continue;

		// Update time left
		if (Buildings[b]->work_time_left > 0) {
			Buildings[b]->work_time_left -= delta_time;
		}

		// Sort by building type
		switch (Buildings[b]->type) {

			// Crafters
			case BUILDING_CRAFTER_1: {

				// If it has active recipe
				if (Buildings[b]->recipe != RECIPE_NONE) {

					// if it has enough power - feature to be added

					// Check state
						// If idle -> start crafting request and switch state to running
						// If active -> wait for request to end

					switch (Buildings[b]->state) {
					case BUILDING_STATE_IDLE: {

						// If output inventory has enough space start craft request
						int allow_craft_request = TRUE;

						// Loop through every output recipe item and chek if it has enough space
						for (int item = 0; item < CraftingRecipes[Buildings[b]->recipe]->output_count; item++) {

							if (!Inventory_enough_space(Buildings[b]->output_inv, CraftingRecipes[Buildings[b]->recipe]->output_items[item].type, CraftingRecipes[Buildings[b]->recipe]->output_items[item].quantity)) {
								allow_craft_request = FALSE;
							}

						}

						// If it has continue with crafting
						if (allow_craft_request) {

							// Start craft request and save id so it can track time left
							int craft_request_id = craft_item(Buildings[b]->input_inv, Buildings[b]->output_inv, Buildings[b]->recipe);
							if (craft_request_id != -1) {
								Buildings[b]->craft_request_id = craft_request_id;
								Buildings[b]->state = BUILDING_STATE_RUNNING;
							}

						}
						break;
					}
					case BUILDING_STATE_RUNNING: {
						if (CraftingQueue[Buildings[b]->craft_request_id].time_left <= 0) {
							Buildings[b]->state = BUILDING_STATE_IDLE;
							Buildings[b]->craft_request_id = NULL;
						}
						break;
					}
					case BUILDING_STATE_PAUSED: {
						// Pause if output inventory is full

					}
					}

				}
				// No selected recipe -> reset building
				else {

				}

				break;

			}
			
			// Miner
			case BUILDING_BURNER_MINER: {


					// if it has enough power - feature to be added

					// Check state
						// If idle -> start crafting request and switch state to running
						// If active -> wait for request to end
										
					switch (Buildings[b]->state) {
					case BUILDING_STATE_IDLE: {

						// If it has ore under itself start working

						struct OreList* Ores[MINER_WIDTH * MINER_HEIGHT] = { NULL };

						int ore_count = getOres(Buildings[b]->coords->x, Buildings[b]->coords->y, Buildings[b]->tile_width, Buildings[b]->tile_height, Ores);


						if (hasOre(Ores)) {

							// Select random ore tile from the list
							// Assign recipe to building

							int rand_ore = SDL_rand(ore_count);

							switch (Ores[rand_ore]->ore.type) {
							case ORE_IRON:
								Buildings[b]->recipe = RECIPE_IRON_ORE;
								break;

							case ORE_COPPER:
								Buildings[b]->recipe = RECIPE_COPPER_ORE;
								break;

							case ORE_COAL:
								Buildings[b]->recipe = RECIPE_COAL_ORE;
								break;
							case ORE_STONE: 
								Buildings[b]->recipe = RECIPE_STONE_ORE;
								break;
							}

							//printf("Recipe set: %d ", Buildings[b]->recipe);

							//printf("Building state: %d \n", Buildings[b]->state);


							// If output inventory has enough space start craft request
							int allow_craft_request = TRUE;

							// Loop through every output recipe item and chek if it has enough space
							for (int item = 0; item < CraftingRecipes[Buildings[b]->recipe]->output_count; item++) {

								if (!Inventory_enough_space(Buildings[b]->output_inv, CraftingRecipes[Buildings[b]->recipe]->output_items[item].type, CraftingRecipes[Buildings[b]->recipe]->output_items[item].quantity)) {
									allow_craft_request = FALSE;
								}

							}

							// If it has continue with crafting
							if (allow_craft_request) {

								// Start craft request and save id so it can track time left
								int craft_request_id = craft_item(Buildings[b]->input_inv, Buildings[b]->output_inv, Buildings[b]->recipe);
								if (craft_request_id != -1) {
									Buildings[b]->craft_request_id = craft_request_id;
									Buildings[b]->state = BUILDING_STATE_RUNNING;
									//printf("crafting recipe: %d\n", CraftingQueue[craft_request_id].recipe);

									//printf("Building state: %d \n", Buildings[b]->state);
								}

							}
						}
						break;
					}
					case BUILDING_STATE_RUNNING: {
						if (CraftingQueue[Buildings[b]->craft_request_id].time_left <= 0) {
							Buildings[b]->state = BUILDING_STATE_IDLE;
							Buildings[b]->craft_request_id = NULL;
						}
						break;
					}
					case BUILDING_STATE_PAUSED: {
						// Pause if output inventory is full

					}
				}
	
				break;
			}

			// Smelter
			case BUILDING_BURNER_SMELTER: {

				// If it has active recipe
				if (Buildings[b]->recipe != RECIPE_NONE) {

					// if it has enough power - feature to be added

					// Check state
						// If idle -> start crafting request and switch state to running
						// If active -> wait for request to end

					switch (Buildings[b]->state) {
					case BUILDING_STATE_IDLE: {

						// If output inventory has enough space start craft request
						int allow_craft_request = TRUE;

						// Loop through every output recipe item and chek if it has enough space
						for (int item = 0; item < CraftingRecipes[Buildings[b]->recipe]->output_count; item++) {

							if (!Inventory_enough_space(Buildings[b]->output_inv, CraftingRecipes[Buildings[b]->recipe]->output_items[item].type, CraftingRecipes[Buildings[b]->recipe]->output_items[item].quantity)) {
								allow_craft_request = FALSE;
							}

						}

						// If it has continue with crafting
						if (allow_craft_request) {

							// Start craft request and save id so it can track time left
							int craft_request_id = craft_item(Buildings[b]->input_inv, Buildings[b]->output_inv, Buildings[b]->recipe);
							if (craft_request_id != -1) {
								Buildings[b]->craft_request_id = craft_request_id;
								Buildings[b]->state = BUILDING_STATE_RUNNING;
							}

						}
						break;
					}
					case BUILDING_STATE_RUNNING: {
						if (CraftingQueue[Buildings[b]->craft_request_id].time_left <= 0) {
							Buildings[b]->state = BUILDING_STATE_IDLE;
							Buildings[b]->craft_request_id = NULL;
						}
						break;
					}
					case BUILDING_STATE_PAUSED: {
						// Pause if output inventory is full

					}
					}

				}
				// No selected recipe -> reset building
				else {

				}

				break;
			}

			// Inserters
			case BUILDING_INSERTER_LONG:
			case BUILDING_INSERTER: {

				// If it has building in front and in back  
					// If idle
						// If building in front is crafter/smelter check if recipe has enough items -> if so dont move
						// Take last item from output inventory of back building
						// Transfer it to input inv of front building
						// Start cooldown and switch to running state
					// If running
						// If time left = 0 set to idle

				// Get back building and front building
				int back_x_offset, back_y_offset, front_x_offset, front_y_offset;

				
				switch (Buildings[b]->rotation) 
				{
				case DOWN:
					back_x_offset = 0;
					back_y_offset = 1;
					front_x_offset = 0;
					front_y_offset = -1;
					break;
				case LEFT:
					back_x_offset = 1;
					back_y_offset = 0;
					front_x_offset = -1;
					front_y_offset = 0;
					break;
				case UP:
					back_x_offset = 0;
					back_y_offset = -1;
					front_x_offset = 0;
					front_y_offset = 1;
					break;
				case RIGHT:
					back_x_offset = -1;
					back_y_offset = 0;
					front_x_offset = 1;
					front_y_offset = 0;
					break;

				default:
					break;
				}

				// In case its a long inserter multiply offset by 2
				if (Buildings[b]->type == BUILDING_INSERTER_LONG) {

					back_x_offset = back_x_offset * 2;
					back_y_offset = back_y_offset * 2;
					front_x_offset = front_x_offset * 2;
					front_y_offset = front_y_offset * 2;
				}

				int back_x = Buildings[b]->coords->x + back_x_offset;
				int back_y = Buildings[b]->coords->y + back_y_offset;
				int front_x = Buildings[b]->coords->x + front_x_offset;
				int front_y = Buildings[b]->coords->y + front_y_offset;

				// Get from and to building pointer
				struct Building* from_building = NULL;
				struct Building* to_building = NULL;


				for (int i = 0; i < MAX_BUILDINGS; i++) {

					if (Buildings[i] == NULL) continue;
					if (Buildings[i]->type == BUILDING_INSERTER || Buildings[i]->type == BUILDING_INSERTER_LONG) continue;

					// From building
					if (Building_is_inside(Buildings[i], back_x, back_y)) {
						from_building = Buildings[i];
					}

					// To building
					else if (Building_is_inside(Buildings[i], front_x, front_y)) {
						to_building = Buildings[i];
					}

				}

				if (from_building && to_building) {

					
					switch (Buildings[b]->state)
					{
					case BUILDING_STATE_IDLE: {
						int from_index = Inventory_get_last_item_index(from_building->output_inv);

						// If to inventory has enough space and the to building isn't working transfer 1 item
						if (from_index != -1) {

							//printf("Item type: %d, slot index: %d\n", from_building->output_inv->slots[from_index].type, from_index);

							// In case of transfering to a conveyor belt
							if (to_building->type == BUILDING_CONVEYOR) {

								if (to_building->output_inv->slots[0].type == ITEM_NONE) {

									Inventory_transfer_item(
										from_building->output_inv,
										to_building->output_inv,
										0,
										1
									);
								}
							}
							
							else if (Inventory_enough_space(
								to_building->input_inv, from_building->output_inv->slots[from_index].type, 1) 
								&& to_building->state == BUILDING_STATE_IDLE 
								&& !Inventory_enough_item_for_recipe(to_building->input_inv, to_building->recipe, from_building->output_inv->slots[from_index]) 
								&& to_building->recipe != RECIPE_NONE) 
							
							{

								// if building has enouugh of the currently selected item for the recipe dont transfer
								printf("inserter working");

								Inventory_transfer_item(
									from_building->output_inv,
									to_building->input_inv,
									from_index,
									1
								);

								Buildings[b]->state = BUILDING_STATE_RUNNING;
								Buildings[b]->work_time_left = INSERTER_WORK_TIME;
							}
						}
					}

					case BUILDING_STATE_RUNNING: {

						if (Buildings[b]->work_time_left <= 0) {

							Buildings[b]->state = BUILDING_STATE_IDLE;
						}

					}

					default:
						break;
					}

				}

				break;
			}

			// Conveyor belt
			case BUILDING_CONVEYOR: {

				// Move item from its own inventory to the next conveyor building if it's free
				
				// Get back building and front building
				int front_x_offset, front_y_offset;
				switch (Buildings[b]->rotation)
				{
				case DOWN:
					front_x_offset = 0;
					front_y_offset = -1;
					break;
				case LEFT:
					front_x_offset = -1;
					front_y_offset = 0;
					break;
				case UP:
					front_x_offset = 0;
					front_y_offset = 1;
					break;
				case RIGHT:
					front_x_offset = 1;
					front_y_offset = 0;
					break;

				default:
					break;
				}

				int front_x = Buildings[b]->coords->x + front_x_offset;
				int front_y = Buildings[b]->coords->y + front_y_offset;

				// Get from and to building pointer
				struct Building* to_building = NULL;

				for (int i = 0; i < MAX_BUILDINGS; i++) {

					if (Buildings[i] == NULL) continue;
					// Skip if next building is not conveyor
					if (Buildings[i]->type != BUILDING_CONVEYOR) continue;

					// To building
					if (Building_is_inside(Buildings[i], front_x, front_y)) {
						to_building = Buildings[i];
					}
				}

				if (to_building) {

					switch (Buildings[b]->state)
					{
					case BUILDING_STATE_IDLE: {

						// If next conveyor has no items and this one has, transfer it
						if (to_building->output_inv->slots[0].type == ITEM_NONE && Buildings[b]->output_inv->slots[0].type != ITEM_NONE) {

							Buildings[b]->state = BUILDING_STATE_RUNNING;
							Buildings[b]->work_time_left = CONVEYOR_WORK_TIME;
						}

						break;
					}
					case BUILDING_STATE_RUNNING: {

						if (Buildings[b]->work_time_left <= 0) {

							Inventory_transfer_item(Buildings[b]->output_inv, to_building->output_inv, 0, 1);
							Buildings[b]->state = BUILDING_STATE_IDLE;
						}

						break;
					}
					default:
						break;
					}
				}

				break;
			}
		}
	}
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
		
		// Set render color depending on building

		unsigned int rgba_colors[4] = { 0 };
		Hex2RGBA(b->color, rgba_colors);

		// Set render color
		SDL_SetRenderDrawColor(renderer, rgba_colors[0], rgba_colors[1], rgba_colors[2], rgba_colors[3]);


		// Add to render  
		SDL_RenderFillRect(renderer, &building_rect);

		// Draw square on top of inserter and coveyor to determine facing direction
		if (b->type == BUILDING_INSERTER || b->type == BUILDING_CONVEYOR || b->type == BUILDING_INSERTER_LONG) {

			SDL_FRect arrow_rect;

			switch (b->rotation) {
			case DOWN:
				arrow_rect.x = building_rect.x + TILE_SIZE / 2 - 5;
				arrow_rect.y = building_rect.y + TILE_SIZE - 10;
				arrow_rect.w = 10;
				arrow_rect.h = 10;
				break;
			case LEFT:
				arrow_rect.x = building_rect.x;
				arrow_rect.y = building_rect.y + TILE_SIZE / 2 - 5;
				arrow_rect.w = 10;
				arrow_rect.h = 10;
				break;
			case UP:
				arrow_rect.x = building_rect.x + TILE_SIZE / 2 - 5;
				arrow_rect.y = building_rect.y;
				arrow_rect.w = 10;
				arrow_rect.h = 10;
				break;
			case RIGHT:
				arrow_rect.x = building_rect.x + TILE_SIZE - 10;
				arrow_rect.y = building_rect.y + TILE_SIZE / 2 - 5;
				arrow_rect.w = 10;
				arrow_rect.h = 10;
				break;
			}


			SDL_SetRenderDrawColor(renderer, 38, 176, 255, 255);

			SDL_RenderFillRect(renderer, &arrow_rect);

		}

		// Draw item on top of conveyor if it has one
		if (b->type == BUILDING_CONVEYOR && b->output_inv->slots[0].type != ITEM_NONE) {

			SDL_FRect item_rect = {
				building_rect.x + TILE_SIZE / 2 - 8,
				building_rect.y + TILE_SIZE / 2 - 8,
				16,
				16
			};

			unsigned int rgba_colors[4] = { 0 };
			Hex2RGBA(Item_data_list[b->output_inv->slots[0].type]->color, rgba_colors);

			// Set render color
			SDL_SetRenderDrawColor(renderer, rgba_colors[0], rgba_colors[1], rgba_colors[2], rgba_colors[3]);

			SDL_RenderFillRect(renderer, &item_rect);


		}

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


// Return true if coordinate is inside building
int Building_is_inside(struct Building* b, int x_coord, int y_coord) {

	if (x_coord >= b->coords->x && x_coord < b->coords->x + b->tile_width && y_coord >= b->coords->y && y_coord < b->coords->y + b->tile_height) return TRUE;
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