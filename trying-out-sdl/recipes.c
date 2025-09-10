#include <stdio.h>   
#include <stdlib.h>  
#include <string.h>

#include "cJSON.h"
#include "item.h"
#include "recipes.h"


// Init recipes from predefined set of variables
// Add to array of all recipes
struct CraftingRecipe* CraftingRecipes[MAX_RECIPES] = { NULL };
int recipe_count = 0;

void init_recipes() {

    recipe_load_from_json("recipes.json");
}

// Returns TRUE if provided method is available for the recipe, else FALSE
int recipe_match_method(struct CraftingRecipe* recipe, enum RecipeCraftMethod craft_method) {
    
    // Loop through all possible crafting methods
    for (int m = 0; m < CRAFT_METHODS; m++) {

        if (!recipe) continue;
        if (recipe->craft_method[m] == craft_method) return TRUE;
    }

    // If no match is found return false
    return FALSE;

}

void recipe_load_from_json(const char* filename) {  
   FILE* f = fopen(filename, "rb");  
   if (!f) { perror("Failed to open JSON file"); exit(1); }  

   fseek(f, 0, SEEK_END);  
   long size = ftell(f);  
   rewind(f);  

   char* data = malloc(size + 1);  

   if (!data) {  
       perror("Failed to allocate memory for JSON data");  
       fclose(f);  
       exit(1);  
   }  

   fread(data, 1, size, f);  
   data[size] = '\0';  
   fclose(f);  

   cJSON* root = cJSON_Parse(data);  
   free(data);  
   if (!root) {  
       printf("Failed to parse JSON\n");  
       return;  
   }  

   int n = cJSON_GetArraySize(root);  
   for (int i = 0; i < n && recipe_count < MAX_RECIPES; i++) {  
       cJSON* item = cJSON_GetArrayItem(root, i);  

       // allocate memory for the recipe
       CraftingRecipes[recipe_count] = malloc(sizeof(struct CraftingRecipe));
       if (!CraftingRecipes[recipe_count]) {
           perror("Failed to allocate memory for CraftingRecipe");
           exit(1);
       }

       CraftingRecipes[recipe_count]->name = str_to_recipe_name(cJSON_GetObjectItem(item, "name")->valuestring);  
       CraftingRecipes[recipe_count]->state = str_to_recipe_state(cJSON_GetObjectItem(item, "state")->valuestring);
       CraftingRecipes[recipe_count]->crafting_time = (float)cJSON_GetObjectItem(item, "crafting_time")->valuedouble;
       CraftingRecipes[recipe_count]->required_energy = (float)cJSON_GetObjectItem(item, "required_energy")->valuedouble;

       // Copy title string into title
       const char* title_str = cJSON_GetObjectItem(item, "title")->valuestring;
       CraftingRecipes[recipe_count]->title = _strdup(title_str);

       printf("Recipe %d : %s\n", i, CraftingRecipes[i]->title);

       // Crafting methods  
       cJSON* methods = cJSON_GetObjectItem(item, "craft_method");  
       for (int m = 0; m < CRAFT_METHODS; m++) {  
           CraftingRecipes[recipe_count]->craft_method[m] = RECIPE_HAND; // Default  
       }  

       int method_count = cJSON_GetArraySize(methods);  
       for (int j = 0; j < method_count && j < CRAFT_METHODS; j++) {  
           CraftingRecipes[recipe_count]->craft_method[j] = str_to_craft_method(cJSON_GetArrayItem(methods, j)->valuestring);
       }  

       // Input items  
       cJSON* inputs = cJSON_GetObjectItem(item, "input");  
       int input_count = cJSON_GetArraySize(inputs);  
       CraftingRecipes[recipe_count]->input_count = input_count;
       CraftingRecipes[recipe_count]->input_items = malloc(sizeof(struct RecipeItem) * input_count);
       for (int j = 0; j < input_count; j++) {  
           cJSON* ri = cJSON_GetArrayItem(inputs, j);  
           CraftingRecipes[recipe_count]->input_items[j].type = str_to_item_type(cJSON_GetObjectItem(ri, "type")->valuestring);
           CraftingRecipes[recipe_count]->input_items[j].quantity = cJSON_GetObjectItem(ri, "quantity")->valueint;
       }  

       // Output items  
       cJSON* outputs = cJSON_GetObjectItem(item, "output");  
       int output_count = cJSON_GetArraySize(outputs);  
       CraftingRecipes[recipe_count]->output_count = output_count;
       CraftingRecipes[recipe_count]->output_items = malloc(sizeof(struct RecipeItem) * output_count);
       for (int j = 0; j < output_count; j++) {  
           cJSON* ri = cJSON_GetArrayItem(outputs, j);  
           CraftingRecipes[recipe_count]->output_items[j].type = str_to_item_type(cJSON_GetObjectItem(ri, "type")->valuestring);
           CraftingRecipes[recipe_count]->output_items[j].quantity = cJSON_GetObjectItem(ri, "quantity")->valueint;
       }  


       recipe_count++;  
   }  

   cJSON_Delete(root);  
}

// Maps for converting JSON to enum
typedef struct { const char* name; enum RecipeName value; } RecipeNameMap;
typedef struct { const char* name; enum RecipeState value; } RecipeStateMap;
typedef struct { const char* name; enum RecipeCraftMethod value; } RecipeCraftMethodMap;
typedef struct { const char* name; enum ItemType value; } ItemTypeMap;

// Mapping instructions
static const RecipeNameMap recipe_name_map[] = {
    { "RECIPE_IRON_PLATE", RECIPE_IRON_PLATE },
    { "RECIPE_COPPER_PLATE", RECIPE_COPPER_PLATE },
    { "RECIPE_IRON_GEAR", RECIPE_IRON_GEAR },
    { "RECIPE_NONE", RECIPE_NONE }
};

static const RecipeStateMap recipe_state_map[] = {
    { "RECIPE_LOCKED", RECIPE_LOCKED },
    { "RECIPE_RESEARCHING", RECIPE_RESEARCHING },
    { "RECIPE_AVAILABLE", RECIPE_AVAILABLE }
};

static const RecipeCraftMethodMap craft_method_map[] = {
    { "RECIPE_HAND", RECIPE_HAND },
    { "RECIPE_CRAFTER_1", RECIPE_CRAFTER_1 },
    { "RECIPE_CRAFTER_2", RECIPE_CRAFTER_2 },
    { "RECIPE_SMELTER", RECIPE_SMELTER }
};

static const ItemTypeMap item_type_map[] = {
    { "ITEM_IRON_ORE", ITEM_IRON_ORE },
    { "ITEM_COPPER_ORE", ITEM_COPPER_ORE },
    { "ITEM_IRON_PLATE", ITEM_IRON_PLATE },
    { "ITEM_COPPER_PLATE", ITEM_COPPER_PLATE },
    { "ITEM_IRON_GEAR", ITEM_IRON_GEAR },
    { "ITEM_NONE", ITEM_NONE }
};

// Convert string to enum and return it
static enum RecipeName str_to_recipe_name(const char* str) {
    for (size_t i = 0; i < sizeof(recipe_name_map) / sizeof(recipe_name_map[0]); i++) {
        if (strcmp(str, recipe_name_map[i].name) == 0) return recipe_name_map[i].value;
    }
    return RECIPE_NONE;
}

static enum RecipeState str_to_recipe_state(const char* str) {
    for (size_t i = 0; i < sizeof(recipe_state_map) / sizeof(recipe_state_map[0]); i++) {
        if (strcmp(str, recipe_state_map[i].name) == 0) return recipe_state_map[i].value;
    }
    return RECIPE_LOCKED;
}

static enum RecipeCraftMethod str_to_craft_method(const char* str) {
    for (size_t i = 0; i < sizeof(craft_method_map) / sizeof(craft_method_map[0]); i++) {
        if (strcmp(str, craft_method_map[i].name) == 0) return craft_method_map[i].value;
    }
    return RECIPE_HAND;
}

static enum ItemType str_to_item_type(const char* str) {
    for (size_t i = 0; i < sizeof(item_type_map) / sizeof(item_type_map[0]); i++) {
        if (strcmp(str, item_type_map[i].name) == 0) return item_type_map[i].value;
    }
    return ITEM_NONE;
}