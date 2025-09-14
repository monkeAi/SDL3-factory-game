#ifndef RECIPES_H
#define RECIPES_H

#include "constants.h"
#include "item.h"

// Crafting Recipes
/*

Recipe name
Made in limits
Input items
Input Required Quantity
Amount of input items
Required energy
Crafting time
Output items
Output Quantity
Amount of output items


*/

// Standard recipe layout
struct CraftingRecipe {
	enum RecipeName name;
	enum RecipeState state;
	enum RecipeCraftMethod craft_method[CRAFT_METHODS];
	unsigned int input_count;
	unsigned int output_count;
	struct RecipeItem* input_items;
	struct RecipeItem* output_items;
	float crafting_time;
	float required_energy;
	char* title;
};

struct RecipeItem {
	enum ItemType type;
	int quantity;
};

// Available recipe crafting methods
enum RecipeCraftMethod {
	RECIPE_M_HAND,
	RECIPE_M_CRAFTER,
	RECIPE_M_SMELTER,
	RECIPE_M_MINER
};

enum RecipeState {
	RECIPE_LOCKED,
	RECIPE_RESEARCHING,
	RECIPE_AVAILABLE
};

// Every possible recipe list
enum RecipeName {
	RECIPE_NONE,

	RECIPE_IRON_ORE,
	RECIPE_IRON_PLATE,
	RECIPE_IRON_GEAR,

	RECIPE_COPPER_ORE,
	RECIPE_COPPER_PLATE,
	RECIPE_COPPER_WIRE,

	RECIPE_COAL_ORE,

};

// Recipe is a list of parameters for the crafting system 

extern struct CraftingRecipe* CraftingRecipes[MAX_RECIPES];

void init_recipes();
void recipe_load_from_json(const char* filename);
int recipe_match_method(struct CraftingRecipe* recipe, enum RecipeCraftMethod craft_method);

static enum RecipeName str_to_recipe_name(const char* str);
static enum RecipeState str_to_recipe_state(const char* str);
static enum RecipeCraftMethod str_to_craft_method(const char* str);
static enum ItemType str_to_item_type(const char* str);

#endif