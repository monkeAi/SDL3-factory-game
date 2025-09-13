#include <stdio.h>	
#include "crafting.h"
#include "recipes.h"
#include "inventory.h"
#include "player.h"

struct CraftRequest CraftingQueue[MAX_CRAFT_QUEUE];
unsigned int craft_queue_head = 0;
//unsigned int craft_queue_tail = 0;

// Crafts a new item from selected recipe, takes it from input inventory and puts it into output inventory, returns the craft request id
int craft_item(struct Inventory* input_inv, struct Inventory* output_inv, enum RecipeName recipe) {

	
	// If there is enough items in the input_inv for the recipe, move them to temporary inventory and start crafting timer
	// Loop through each required item
	for (int item = 0; item < CraftingRecipes[recipe]->input_count; item++) {

		// Return if there is no such item or there isnt enought of it
		int searched_item_slot = Inventory_search_item(input_inv, CraftingRecipes[recipe]->input_items[item].type);
		if (searched_item_slot == -1 || input_inv->slots[searched_item_slot].quantity < CraftingRecipes[recipe]->input_items[item].quantity) {
			//printf("Not enough items.\n");
			return -1;
		}
	}

	// Create a temp inventory and place needed items inside

	struct Inventory* tempInv = NULL;

	tempInv = Inventory_create(CraftingRecipes[recipe]->input_count, CraftingRecipes[recipe]->input_count);

	for (int item = 0; item < CraftingRecipes[recipe]->input_count; item++) {

		Inventory_transfer_item(input_inv, tempInv, Inventory_search_item(input_inv, CraftingRecipes[recipe]->input_items[item].type), CraftingRecipes[recipe]->input_items[item].quantity);
	}

	// For now free temp inventory, in future save temp inventory in case building gets deconstructed

	Inventory_free(tempInv);
	

	// Create a new crafting request and add it to the queue
	//printf("Starting craft request %d\n", craft_queue_head);
	struct CraftRequest request = craft_request_create(output_inv, recipe);
	CraftingQueue[craft_queue_head] = request;
	int request_id = craft_queue_head;
	craft_queue_head = (craft_queue_head + 1) % MAX_CRAFT_QUEUE;

	return request_id;

	// After crafting time is complete return items from temp inventory to output inventory
}


// Loop through entire crafting queue and subtract delta time from each craft request, when done delete temp inventory and add new item to the output inventory
void update_craft_queue(float delta_time) {

	for (unsigned int request = 0; request < MAX_CRAFT_QUEUE; request++) {

		// Skip if request is empty
		if (CraftingQueue[request].recipe == RECIPE_NONE) continue;

		if (!CraftingQueue[request].output_inv) continue;

		CraftingQueue[request].time_left -= delta_time;

		// When crafting time is over 
		if (CraftingQueue[request].time_left <= 0) {

			// Create output items and push them to the new inventory
			for (unsigned int item = 0; item < CraftingRecipes[CraftingQueue[request].recipe]->output_count; item++) {

				struct Item output_item = Item_create(CraftingRecipes[CraftingQueue[request].recipe]->output_items[item].type, CraftingRecipes[CraftingQueue[request].recipe]->output_items[item].quantity);
				


				Inventory_push_item(CraftingQueue[request].output_inv, &output_item);
				// Handle full inventory case

				// If output inventory is full then stop crafting queue
				// Store crafted items in temporary inventory
			}

			// Remove craft request from queue
			//printf("%d Item crafted. \n", request);
			craft_request_delete(request);
		}
	}

	// Update player queue

	update_waiting_queue(delta_time);

	return;
}


// Creates a new craft request
static struct CraftRequest craft_request_create(struct Inventory* output_inv, enum RecipeName recipe) {
	struct CraftRequest request;
	request.recipe = recipe;
	request.output_inv = output_inv;
	request.time_left = CraftingRecipes[recipe]->crafting_time;
	return request;
}

static void craft_request_delete(unsigned int request) {

	CraftingQueue[request].recipe = RECIPE_NONE;
	CraftingQueue[request].output_inv = NULL;
	CraftingQueue[request].time_left = 0;
}

// Player crafting queue

void add_recipe_to_queue(enum RecipeName recipe) {

	struct Inventory* input_inv = player->inventory;
	struct Inventory* output_inv = player->inventory;


	// If there is enough items in the input_inv for the recipe, move them to temporary inventory and start crafting timer
	// Loop through each required item
	for (int item = 0; item < CraftingRecipes[recipe]->input_count; item++) {

		// Return if there is no such item or there isnt enought of it
		int searched_item_slot = Inventory_search_item(input_inv, CraftingRecipes[recipe]->input_items[item].type);
		if (searched_item_slot == -1 || input_inv->slots[searched_item_slot].quantity < CraftingRecipes[recipe]->input_items[item].quantity) {
			//printf("Not enough items.\n");
			return -1;
		}
	}

	// Create a temp inventory and place needed items inside

	struct Inventory* tempInv = NULL;

	tempInv = Inventory_create(CraftingRecipes[recipe]->input_count, CraftingRecipes[recipe]->input_count);

	for (int item = 0; item < CraftingRecipes[recipe]->input_count; item++) {

		Inventory_transfer_item(input_inv, tempInv, Inventory_search_item(input_inv, CraftingRecipes[recipe]->input_items[item].type), CraftingRecipes[recipe]->input_items[item].quantity);
	}

	// For now free temp inventory, in future save temp inventory in case building gets deconstructed

	Inventory_free(tempInv);

	// Create a new WaitingCraftRequest item and add it to the end of the requests
	struct WaitingCraftRequest* new_request = malloc(sizeof(struct WaitingCraftRequest));
	new_request->recipe = recipe;
	new_request->output_inv = output_inv;
	new_request->next_request = NULL;
	new_request->time_left = CraftingRecipes[recipe]->crafting_time;

	// Find the last request of player and add the new request to end

	struct WaitingCraftRequest* curr_request = player->craft_request;

	if (curr_request == NULL) {
		player->craft_request = new_request;
	}
	else {

		while (curr_request->next_request != NULL) {

			curr_request = curr_request->next_request;
		}

		curr_request->next_request = new_request;

	}
}


// Handle player queue

void update_waiting_queue(float delta_time) {

	// When current request completes set current to next and add item

	struct WaitingCraftRequest* curr_request = player->craft_request;

	if (!curr_request) return;



	curr_request->time_left -= delta_time;

	if (curr_request->time_left <= 0) {

		printf("Queued craft request completed!\n");

		// Create output items and push them to the new inventory
		for (unsigned int item = 0; item < CraftingRecipes[curr_request->recipe]->output_count; item++) {

			struct Item output_item = Item_create(CraftingRecipes[curr_request->recipe]->output_items[item].type, CraftingRecipes[curr_request->recipe]->output_items[item].quantity);

			Inventory_push_item(curr_request->output_inv, &output_item);

			// Handle full inventory case
		}

		player->craft_request = player->craft_request->next_request;
	}
}
