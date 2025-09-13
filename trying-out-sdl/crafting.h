#ifndef CRAFTING_H
#define CRAFTING_H

#include "constants.h"


struct CraftRequest {
	enum RecipeName recipe;
	struct Inventory* output_inv;
	float time_left;
};

struct WaitingCraftRequest {
	enum RecipeName recipe;
	struct Inventory* output_inv;
	float time_left;
	struct WaitingCraftRequest* next_request;
};

extern struct CraftRequest CraftingQueue[MAX_CRAFT_QUEUE];


int craft_item(struct Inventory* input_inv, struct Inventory* output_inv, enum RecipeName recipe);
struct CraftRequest craft_request_create(struct Inventory* output_inv, enum RecipeName recipe);
void craft_request_delete(unsigned int request);
void update_craft_queue(float delta_time);

// Player queue
void add_recipe_to_queue(enum RecipeName recipe);
void update_waiting_queue(float delta_time);


#endif