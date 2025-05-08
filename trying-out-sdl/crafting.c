#include <stdio.h>	
#include "crafting.h"
#include "recipes.h"
#include "inventory.h"

struct CraftRequest CraftingQueue[MAX_CRAFT_QUEUE];
unsigned int craft_queue_head = 0;
//unsigned int craft_queue_tail = 0;

// Crafts a new item from selected recipe, takes it from input inventory and puts it into output inventory
void craft_item(struct Inventory* input_inv, struct Inventory* output_inv, enum RecipeName recipe) {

	printf("Craft item \n");
	// If there is enough items in the input_inv for the recipe, move them to temporary inventory and start crafting timer
	// Loop through each required item
	for (int item = 0; item < CraftingRecipes[recipe].input_count; item++) {
		// Return if there is no such item
		if (Inventory_search_item(input_inv, CraftingRecipes[recipe].input_items[item].type) == -1) return;
	}

	printf("Enough ingredients \n");

	// Create a temp inventory and place needed items inside

	struct Inventory* tempInv = NULL;

	tempInv = Inventory_create(CraftingRecipes[recipe].input_count);

	for (int item = 0; item < CraftingRecipes[recipe].input_count; item++) {

		//Inventory_push_item(tempInv, &input_inv->slots[Inventory_search_item(input_inv, CraftingRecipes[recipe].input_items[item].type)]);	// Ne sme pushat usega ampak samo requred amount
		Inventory_transfer_item(input_inv, tempInv, Inventory_search_item(input_inv, CraftingRecipes[recipe].input_items[item].type), CraftingRecipes[recipe].input_items[item].quantity);
	}
	
	// After crafting time is complete return items from temp inventory to output inventory

	printf("Starting craft request \n");
	struct CraftRequest request = craft_request_create(output_inv, recipe);
	CraftingQueue[craft_queue_head] = request;
	craft_queue_head = (craft_queue_head + 1) % MAX_CRAFT_QUEUE;
}


// Loop through entire crafting queue and subtract delta time from each craft request, when done delete temp inventory and add new item to the output inventory
void update_craft_queue(float delta_time) {

	for (unsigned int request = 0; request < MAX_CRAFT_QUEUE; request++) {

		// Skip if request is empty
		if (CraftingQueue[request].recipe == RECIPE_NONE) continue;

		CraftingQueue[request].time_left -= delta_time;

		// When crafting time is over 
		if (CraftingQueue[request].time_left <= 0) {

			// Create output items and push them to the new inventory
			for (unsigned int item = 0; item < CraftingRecipes[CraftingQueue[request].recipe].output_count; item++) {

				unsigned int max_quantity = 100;	// Change later to be dependant on item type

				struct Item output_item = Item_create(CraftingRecipes[CraftingQueue[request].recipe].output_items[item].type, max_quantity, CraftingRecipes[CraftingQueue[request].recipe].output_items[item].quantity);
				
				Inventory_push_item(CraftingQueue[request].output_inv, &output_item);
				// Handle full inventory case
			}

			// Remove craft request from queue
			//craft_queue_tail = (craft_queue_tail + 1) % MAX_CRAFT_QUEUE;
			printf("Item crafted. \n");
			craft_request_delete(request);
		}
	}

	return;
}


// Creates a new craft request
static struct CraftRequest craft_request_create(struct Inventory* output_inv, enum RecipeName recipe) {
	struct CraftRequest request;
	request.recipe = recipe;
	request.output_inv = output_inv;
	request.time_left = CraftingRecipes[recipe].crafting_time;
	return request;
}

static void craft_request_delete(unsigned int request) {
	CraftingQueue[request].recipe = RECIPE_NONE;
	CraftingQueue[request].output_inv = NULL;
	CraftingQueue[request].time_left = 0;

	// Po moznosti free
}

// Somehow add variable recipes for crafting time boosts and other boosts
