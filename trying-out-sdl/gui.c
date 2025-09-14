#include <stdio.h>	
#include "gui.h"
#include "tools.h"
#include "inventory.h"
#include "player.h"
#include "media.h"
#include "recipes.h"
#include "buildings.h"
#include "crafting.h"

// Main GUI frame


// inventory gui

// Building gui

// Interaction gui

// Toolbar gui

// Game Menu gui

// Table of all gui window instances
struct GUI_frame* GUI_WINDOWS[MAX_GUI_WINDOWS] = { NULL };

// Init all importat gui
void init_gui() {
	
}

// Initialize a GUI frame
static struct GUI_frame* gui_frame_init(struct GUI_frame* parent, const int max_children) {

	// Allocate enough memory to the building pointer
	struct GUI_frame* frame = (struct GUI_frame*)malloc(sizeof(struct GUI_frame));
	if (frame == NULL) {
		fprintf(stderr, "Failed to allocate memory for gui frame.\n");
		exit(1);
	}

	// Init position relative to parent position
	frame->parent = parent;
	frame->max_children = max_children;
	frame->children = calloc(max_children, sizeof(struct GUI_frame*));

	frame->position[0] = 0;
	frame->position[1] = 0;

	frame->height = 50;
	frame->width = 50;

	for (int i = 0; i < MAX_GUI_CLASS; i++) {

		frame->class[i] = C_NONE;
	}

	frame->id = 0;

	frame->textBox = malloc(sizeof(struct TextBox));
	frame->text_enabled = FALSE;

	// Background color
	frame->default_color = 0xFFFFFFFF;
	frame->set_color = frame->default_color;

	frame->visibility = SHOWN;

	frame->set_recipe = RECIPE_NONE;

	// If parent isnt NULL then addapt some parameters from it
	if (parent != NULL) {

		// Relative positioning to parent
		frame->position[0] += frame->parent->position[0];
		frame->position[1] += frame->parent->position[1];

		// Add this struct as a child in parents struct
		if (gui_add_child(parent, frame)) {
			printf("Child added unsuccesfully \n");
		}
	}

	return frame;

}

// Free and delete gui frame
void gui_frame_destroy(struct GUI_frame* frame) {
	if (!frame) return;

	// Recursively destroy children first
	if (frame->children && frame->max_children > 0) {
		for (int i = 0; i < frame->max_children; i++) {
			struct GUI_frame* child = frame->children[i];
			if (child) {
				gui_frame_destroy(child);
				frame->children[i] = NULL;   // prevent stale pointer
			}
		}
		free(frame->children);
		frame->children = NULL;
	}

	// If this frame is linked to a parent, clear that slot too
	if (frame->parent && frame->parent->children) {
		for (int i = 0; i < frame->parent->max_children; i++) {
			if (frame->parent->children[i] == frame) {
				frame->parent->children[i] = NULL;
				break;
			}
		}
	}

	free(frame);
}

// Resizes the provided gui frame
void gui_resize(struct GUI_frame* frame, int width, int height) {
	frame->width = width;
	frame->height = height;
}

// Moves gui frame to a new position
void gui_move(struct GUI_frame* frame, int x, int y, int margin_x, int margin_y, enum GUI_flags* FLAGS) {

	int x_offset = x;
	int	y_offset = y;

	// Go through every flag and apply it
	if (FLAGS != NULL) {
		for (int flag = 0; flag < MAX_GUI_FLAGS; flag++) {
			if (!FLAGS[flag]) continue;

			switch (FLAGS[flag]) {
				case POS_CENTERED_X:
					// If it has parent
					if (frame->parent != NULL) {
						x_offset = frame->parent->width / 2 - frame->width / 2;
					}
					// If its based on window
					else {
						x_offset = WINDOW_WIDTH / 2 - frame->width / 2;
					}
					break;

				case POS_CENTERED_Y:
					// If it has parent
					if (frame->parent != NULL) {
						y_offset = frame->parent->height / 2 - frame->height / 2;
					}
					// If its based on window
					else {
						y_offset = WINDOW_HEIGHT / 2 - frame->height / 2;
					}
					break;

				// Add more cases for TOP, DOWN, LEFT, RIGHT

				case POS_LEFT:
					// If it has parent
					if (frame->parent != NULL) {
						x_offset = margin_x;
					}
					// If its based on window
					else {
						x_offset = margin_x;
					}
					break;

				case POS_RIGHT:
					// If it has parent
					if (frame->parent != NULL) {
						x_offset = frame->parent->width - frame->width - margin_x;
					}
					// If its based on window
					else {
						x_offset = WINDOW_WIDTH - frame->width - margin_x;
					}
					break;

				case POS_TOP:
					// If it has parent
					if (frame->parent != NULL) {
						y_offset = margin_y;
					}
					// If its based on window
					else {
						y_offset = margin_y;
					}
					break;

				case POS_BOTTOM:
					// If it has parent
					if (frame->parent != NULL) {
						y_offset = frame->parent->height - frame->height - margin_y;
					}
					// If its based on window
					else {
						y_offset = WINDOW_HEIGHT - frame->height - margin_y;;
					}
					break;
			}
		}
	}
	


	// Relative positioning to parent
	if (frame->parent != NULL) {

		frame->position[0] = frame->parent->position[0] + x_offset;
		frame->position[1] = frame->parent->position[1] + y_offset;
	}
	// Else relative to window
	else {
		frame->position[0] = x_offset;
		frame->position[1] = y_offset;
	}
}

// Resets clor to default
void gui_reset_color(struct GUI_frame* frame) {
	frame->set_color = frame->default_color;
}

// Sets a new color of frame
void gui_set_color(struct GUI_frame* frame, unsigned int new_color) {
	frame->default_color = new_color;
}

// Add child to a parent, return 1 if not enough space
int gui_add_child(struct GUI_frame* parent, struct GUI_frame* child) {

	for (int c = 0; c < parent->max_children; c++) {
		// If it is NULL then add pointer to child at that space
		if (parent->children[c] == NULL) {
			parent->children[c] = child;
			return 0;
		}
	}

	return 1;

}

// Returns a number of children with that class name under the parent path, writes to a table of pointers fo children with provided class name, it searches recursevly
int gui_find_children(struct GUI_frame* parent, enum GUI_class class_name, struct GUI_frame** matches) {

	// Amount of found mathches
	int matches_count = 0;

	if (!parent) return -1;

	// Loop through every child
	for (int i = 0; i < parent->max_children; i++) {

		// If child is NULL continue
		if (parent->children[i] == NULL) continue;

		int free_space = gui_get_first_index(matches);
		
		// If class name matches and there is still space in the list, add pointer to end of list
		if (gui_has_class(parent->children[i], class_name) && free_space != -1) {
			matches[free_space] = parent->children[i];
			matches_count++;
		}

		// Run function on the children as well
		matches_count += gui_find_children(parent->children[i], class_name, matches);
	}

	return matches_count;
}

// Adds class to the gui frame
void gui_add_class(struct GUI_frame* frame, enum GUI_class class) {

	//printf("Adding class %d to frame: %x\n", class, frame);

	// Find class slot with no class
	for (int c = 0; c < MAX_GUI_CLASS; c++) {
		if (frame->class[c] == C_NONE) {
			frame->class[c] = class;
			//printf("Added class %d to slot %d \n", class, c);
			return;
		}

	}
}

// Returns TRUE if gui_frame contains the provided class
int gui_has_class(struct GUI_frame* frame, enum GUI_class class) {

	for (int c = 0; c < MAX_GUI_CLASS; c++) {
		if (frame->class[c] == class) return TRUE;
	}

	return FALSE;
}

// Returns pointer to gui frame with matching id
struct GUI_frame* gui_get_frame_by_id(struct GUI_frame* parent, enum GUI_ID id) {

	// Loop through every child
	for (int i = 0; i < parent->max_children; i++) {

		// If child is NULL continue
		if (parent->children[i] == NULL) continue;

		// If id matches return struct
		if (parent->children[i]->id == id) {
			return parent->children[i];
		}

		// Run function on the children as well
		return gui_get_frame_by_id(parent->children[i], id);
	}

	// No match
	return NULL;
}

// Find first free space in matches
int gui_get_first_index(struct GUI_frame** matches) {

	for (int j = 0; j < MAX_GUI_CLASS_MATHCHES; j++) {
		if (matches[j] == NULL) return j;
	}

	// If no free space is available
	return -1;
}

// Updates the gui frame and its children
void gui_frame_update(struct GUI_frame* frame) {

	// Return if null or hidden
	if (frame == NULL || frame->visibility == HIDDEN) return;

	// Reset color
	gui_reset_color(frame);


	// Update values
	switch (frame->state) {
		case GUI_HOVERING: {
			frame->set_color = 0x111111FF;
			break;
		}
	}

	frame->state = GUI_NONE;

	// Update children
	for (int c = 0; c < frame->max_children; c++) {
		// If child isnt NULL then render that child as well
		if (frame->children[c] == NULL) continue;
		gui_frame_update(frame->children[c]);
	}

	return;

}

// Updates all gui
void update_gui(SDL_Renderer* renderer, struct MediaBin* mediaBin) {

	// Loop through all visible gui and update it
	for (int i = 0; i < MAX_GUI_WINDOWS; i++) {
		if (GUI_WINDOWS[i] == NULL) continue;
		gui_frame_update(GUI_WINDOWS[i]);
	}


	// Update player holding item position
	if (player->cursor->is_holding) {

		player->cursor->held_item->position[0] = player->mouse_pos[0];
		player->cursor->held_item->position[1] = player->mouse_pos[1];

	}


	// Update GUI Player Inventory
	if (player->gui_inventory->visibility == SHOWN || player->cursor->is_holding) {
		gui_update_inventory(player->gui_inventory, player->inventory, renderer, mediaBin);

		// Update side menu depending on what is open
		switch (player->side_menu_state) {
			case SM_BUILDING: {
			
				player->gui_side_menu->children[0]->visibility = SHOWN;		// Show buildings interface
				player->gui_side_menu->children[1]->visibility = HIDDEN;
				gui_update_sm_buildings(renderer, mediaBin);
				break;
			}
		
			case SM_CRAFTING: {
				player->gui_side_menu->children[0]->visibility = HIDDEN;		// Show buildings interface
				player->gui_side_menu->children[1]->visibility = SHOWN;
				gui_update_sm_crafting(renderer, mediaBin);									// Update crafting
				break;
			}
			
			default: {
				player->gui_side_menu->children[0]->visibility = HIDDEN;		// Show buildings interface
				player->gui_side_menu->children[1]->visibility = SHOWN;
			}
		}
	}

	// Update player mining bar
	if (player->cursor->deconstruct_time_left == PLAYER_DECONSTRUCT_SPEED) {

		player->gui_mining_bar->visibility = HIDDEN;
	}
	else {

		player->gui_mining_bar->visibility = SHOWN;
		gui_update_progress_bar(player->gui_mining_bar->children[0], PLAYER_DECONSTRUCT_SPEED, player->cursor->deconstruct_time_left);
		//printf("%f/%f\n", PLAYER_DECONSTRUCT_SPEED, player->cursor->deconstruct_time_left);
	}


}

// Renders a passed GUI frame and its children
static void gui_frame_render(SDL_Renderer* renderer, struct GUI_frame* frame) {

	// Render parent
	
	// Return if null or hidden
	if (frame == NULL || frame->visibility == HIDDEN) return;

	SDL_FRect frame_rect = {
		frame->position[0],
		frame->position[1],
		frame->width,
		frame->height
	};

	unsigned int rgba_colors[4] = { 0 };
	Hex2RGBA(frame->set_color, rgba_colors);

	// Set render color
	SDL_SetRenderDrawColor(renderer, rgba_colors[0], rgba_colors[1], rgba_colors[2], rgba_colors[3]);

	// Add to render  
	SDL_RenderFillRect(renderer, &frame_rect);

	// Render textbox if text is enabled
	if (frame->text_enabled) {
		//printf("textbox frect:  x: %f; y: %f, w:%f, h:%f \n", frame->textBox->rect->x, frame->textBox->rect->y, frame->textBox->rect->w, frame->textBox->rect->h);
		render_text_box(renderer, frame->textBox);
	}

	// Render children
	for (int c = 0; c < frame->max_children; c++) {
		// If child isnt NULL then render that child as well
		if (frame->children[c] == NULL) continue;
		gui_frame_render(renderer, frame->children[c]);
	}


	return;
}

// Render gui windows and frames
void render_gui(SDL_Renderer* renderer) {

	for (int i = 0; i < MAX_GUI_WINDOWS; i++) {
		if (GUI_WINDOWS[i] == NULL) continue;
		gui_frame_render(renderer, GUI_WINDOWS[i]);
	}

	// Additionaly render held item on top again
	if (player->cursor->is_holding) {

		gui_frame_render(renderer, player->cursor->held_item);
	}
}

// Returns the first free slot in GUI_WINDOWS list
static int gui_find_free_slot() {
	int slot;
	for (slot = 0; GUI_WINDOWS[slot] != NULL && slot < MAX_GUI_WINDOWS; slot++);
	if (slot == MAX_GUI_WINDOWS) return -1;
	return slot;
}

// Returns true if coordinate is inside gui frame
int gui_is_inside_frame(struct GUI_frame* frame, int x, int y) {

	if (x >= frame->position[0] && x <= frame->position[0] + frame->width && y >= frame->position[1] && y <= frame->position[1] + frame->height) return TRUE;
	else return FALSE;

}


// Creates a player inventory with all needed elements
struct GUI_frame* gui_create_player_inventory() {

	int tiles_width = GUI_INVENTORY_WIDTH * GUI_TILE_SIZE + (GUI_INVENTORY_WIDTH - 1) * GUI_TILE_MARGIN;
	int tiles_height = GUI_INVENTORY_HEIGHT * GUI_TILE_SIZE + (GUI_INVENTORY_HEIGHT - 1) * GUI_TILE_MARGIN;

	

	// Create all components and add them together

	// MAIN FRAME
	struct GUI_frame* main_frame = gui_frame_init(NULL, 6);
	gui_resize(main_frame, tiles_width + 4 * GUI_PADDING + GUI_SM_WIDTH, tiles_height + 4 * GUI_PADDING);
	gui_move(main_frame, 100, 100, 0, 0, (enum GUI_flags[]) { POS_CENTERED_X, POS_CENTERED_Y});
	gui_set_color(main_frame, COLOR_HEX_SEC);
	main_frame->visibility = HIDDEN;
	gui_add_class(main_frame, C_inventory);

	// Create all components and add them together
	
	// INVENTORY FRAME BORDER
	struct GUI_frame* inv_frame_border = gui_frame_init(main_frame, 5);
	gui_resize(inv_frame_border, tiles_width + 2 * GUI_PADDING, tiles_height + 2 * GUI_PADDING);
	gui_move(inv_frame_border, 100, 100, GUI_PADDING, 0, (enum GUI_flags[]) { POS_LEFT, POS_CENTERED_Y });
	gui_set_color(inv_frame_border, COLOR_HEX_MAIN);

	// INVENTORY FRAME
	struct GUI_frame* inv_frame = gui_create_tile_box(inv_frame_border, GUI_INVENTORY_WIDTH, GUI_INVENTORY_HEIGHT, GUI_TILE_SIZE, GUI_TILE_SIZE, GUI_TILE_MARGIN, ID_inventory_frame, ID_player_inventory, COLOR_HEX_SEC);

	// Add final frame composition to list of all gui frames
	GUI_WINDOWS[gui_find_free_slot()] = main_frame;

	return main_frame;
}

// Creates a tile box full of frames
struct GUI_frame* gui_create_tile_box(struct GUI_frame* parent, int tiles_x, int tiles_y, int tile_w, int tile_h, int tile_margin, enum GUI_ID tiles_frame_id, enum GUI_ID inventory_id, unsigned int tile_color) {

	// Total size of the container
	int tiles_width = tiles_x * tile_w + (tiles_x - 1) * tile_margin;
	int tiles_height = tiles_y * tile_h + (tiles_y - 1) * tile_margin;


	// Tile box frame
	struct GUI_frame* tiles_frame = gui_frame_init(parent, tiles_x * tiles_y);
	// Set size to grid of tiles inside
	gui_resize(tiles_frame, tiles_width, tiles_height);
	gui_move(tiles_frame, 0, 0, 0, 0, (enum GUI_flags[]) { POS_CENTERED_X, POS_CENTERED_Y });
	gui_set_color(tiles_frame, parent->default_color);
	tiles_frame->id = tiles_frame_id;

	int slot = 0;

	// Create tiles for box frame
	for (int y = 0; y < tiles_y; y++) {
		for (int x = 0; x < tiles_x; x++) {

			// Create a tile
			struct GUI_frame* tile = gui_frame_init(tiles_frame, 1);

			// Set size 
			gui_resize(tile, tile_w, tile_h);
			gui_move(tile, x * tile_w + x * tile_margin, y * tile_h + y * tile_margin, 0, 0, NULL);
			gui_set_color(tile, tile_color);
			gui_add_class(tile, C_inventory_tile);
			tile->id = inventory_id;

			slot++;
		}
	}
}

void gui_create_item(struct GUI_frame* parent, struct Item* item) {

	struct GUI_frame* item_frame = gui_frame_init(parent, 0);
	// Set params
	gui_resize(item_frame, parent->width, parent->height);
	item_frame->default_color = item->color;
	gui_set_color(item_frame, item->color);
	gui_add_class(item_frame, C_inventory_item);
	// Enable text in the frame so it can be rendered
	item_frame->text_enabled = TRUE;
}

// Ale was here
 
// Updates gui and game inventory (inventory sync)
void gui_update_inventory(struct GUI_frame* gui_inv, struct Inventory* game_inv, SDL_Renderer* renderer, struct MediaBin* mediaBin) {

	struct GUI_frame* gui_slots = gui_get_frame_by_id(gui_inv, ID_inventory_frame);


	if (!gui_slots) {
		printf("Null pointer ID");
	}

	// Loop through game inventory and compare it to gui inventory tiles
	for (int slot = 0; slot < game_inv->max_slots; slot++) {

		// Game inv slot is empty and gui inv slot has item
		if (game_inv->slots[slot].type == ITEM_NONE && gui_slots->children[slot]->children[0] != NULL) {

			// Delete item in slot
			//printf("Slot: %d, delete item \n", slot);
			gui_slots->children[slot]->children[0]->text_enabled = FALSE;
			gui_frame_destroy(gui_slots->children[slot]->children[0]);
		}

		// Game inv slot has an item
		else if (game_inv->slots[slot].type != ITEM_NONE) {


			// Gui inv slot is empty
			if (gui_slots->children[slot]->children[0] == NULL) {

				// Create new gui item
				
				//printf("Slot: %d, create gui item \n", slot);
				gui_create_item(gui_slots->children[slot], &game_inv->slots[slot]);

				if (!gui_slots->children[slot]->children[0]) {
					printf("Null gui item pointer");
					continue;
				}
				
				// gui_slots->children[slot]->children[0] --- Item frame adress

				// Converting integer to string for quantity
				char quantity[12];
				intToStr(game_inv->slots[slot].quantity, quantity);

				
				// Set text size and position to item
				SDL_FRect t_rect = {
					gui_slots->children[slot]->children[0]->position[0],
					gui_slots->children[slot]->children[0]->position[1],
					gui_slots->children[slot]->children[0]->width,
					gui_slots->children[slot]->children[0]->height
				};
                
				//printf("x: %f; y: %f, w:%f, h:%f \n", t_rect->x, t_rect->y, t_rect->w, t_rect->h);

				// Update text box with correct quantity
				update_text_box(renderer, gui_slots->children[slot]->children[0]->textBox, mediaBin->font_text, &t_rect, quantity, COLOR_BLACK);
			}

			// Gui inv slot has the right item
			else {


				// Update text box with correct quantity
				char quantity[12];
				intToStr(game_inv->slots[slot].quantity, quantity);


				// Set text size and position to item
				SDL_FRect t_rect = {
					gui_slots->children[slot]->children[0]->position[0],
					gui_slots->children[slot]->children[0]->position[1],
					gui_slots->children[slot]->children[0]->width,
					gui_slots->children[slot]->children[0]->height
				};

				//printf("x: %f; y: %f, w:%f, h:%f \n", t_rect->x, t_rect->y, t_rect->w, t_rect->h);

				// Update text box with correct quantity
				update_text_box(renderer, gui_slots->children[slot]->children[0]->textBox, mediaBin->font_text, &t_rect, quantity, COLOR_BLACK);
			}
		}

		else {
			//printf("Slot: %d, Continue\n", slot);
		}


	}

	// If it finds item in real inventory 
		// if gui tile is empty -> create inv item
		// else update quantity
	// Else skip

}


// When gui is open
	// Get tile of selected inventory tile
	// Check what item it is
	// Store it into player 
	// When player clicks on it it should stick to the mouse
		// Item gets new draw position but is still in inventory until moved to another


// TO DO:
// Ko kliknes na recipe tile ti starta craft queue
		
// Update recipe list with right items
void gui_update_recipe_list(struct GUI_frame* recipe_list, enum RecipeCraftMethod crafting_method) {

	// Get all recipe tiles
	struct GUI_frame* recipe_tiles = gui_get_frame_by_id(recipe_list, ID_recipe_frame);

	if (!recipe_tiles) {
		printf("null pointer\n");
		return;
	}

	int recipe = 0;

	// Loop through all available recipies based on what is open (player inventory, crafter .. )
	for (int tile = 0; tile < GUI_RECIPE_WIDTH * GUI_RECIPE_HEIGHT; tile++) {

		// Crafting recipe is empty/locked/incorrect crafting method and recipe tile has a recipe item -> delete the item
		while (recipe < MAX_RECIPES && (CraftingRecipes[recipe] == NULL || CraftingRecipes[recipe]->state == RECIPE_LOCKED || !recipe_match_method(CraftingRecipes[recipe], crafting_method))) {

			recipe++;
		}

		struct GUI_frame* slot = recipe_tiles->children[tile];
		struct GUI_frame* item = slot->children[0];

		// invalid or out of recipes -> delete item
		if (recipe >= MAX_RECIPES || CraftingRecipes[recipe] == NULL) {
			if (item != NULL) {
				gui_frame_destroy(item);
			}
		}
		else {
		// valid recipe
		// 
			//printf("tile: %d; recipe: %d\n", tile, recipe);

			// if no item -> create it
			if (item == NULL) {
				gui_create_recipe_item(slot, CraftingRecipes[recipe]);
			}
			else {
				// If recipes dont match delete and create a new one
				if (item->set_recipe != CraftingRecipes[recipe]->name) {
					gui_frame_destroy(item);
					gui_create_recipe_item(slot, CraftingRecipes[recipe]);
				}
				// Update item
				else {
					gui_update_recipe_item(item, CraftingRecipes[recipe]);
				}
			}
		}

		recipe++;
	}
}

// Creates a recipe item with info menu
void gui_create_recipe_item(struct GUI_frame* parent, struct CraftingRecipe* recipe) {

	struct GUI_frame* item_frame = gui_frame_init(parent, 1);

	// Set params
	gui_resize(item_frame, parent->width, parent->height);
	item_frame->default_color = Item_data_list[CraftingRecipes[recipe->name]->output_items[0].type]->color;
	gui_set_color(item_frame, Item_data_list[CraftingRecipes[recipe->name]->output_items[0].type]->color);
	item_frame->set_recipe = recipe->name;
	gui_add_class(item_frame, C_recipe_item);
	gui_add_class(item_frame, C_button);
	item_frame->id = ID_recipe_item;

	// Add menu
}

// Update recipe item
void gui_update_recipe_item(struct GUI_frame* item_frame, struct CraftingRecipe* recipe) {

	item_frame->default_color = Item_data_list[CraftingRecipes[recipe->name]->output_items[0].type]->color;
	gui_set_color(item_frame, Item_data_list[CraftingRecipes[recipe->name]->output_items[0].type]->color);
}


// Creates player's side menu gui
struct GUI_frame* gui_create_sm(struct GUI_frame* parent) {
	
	// Create side menu frame
	struct GUI_frame* side_menu = gui_frame_init(parent, 2);
	gui_resize(side_menu, GUI_SM_WIDTH - GUI_PADDING, parent->height - 2 * GUI_PADDING);
	gui_move(side_menu, 100, 100, GUI_PADDING, 0, (enum GUI_flags[]) { POS_RIGHT, POS_CENTERED_Y });
	gui_set_color(side_menu, COLOR_HEX_MAIN);

	// Create buildings interface
	struct GUI_frame* sm_buildings = gui_create_sm_buildings(side_menu);

	// Create crafting interface
	struct GUI_frame* sm_crafting = gui_create_sm_crafting(side_menu);

	return side_menu;
}

// Creates crafting side menu
struct GUI_frame* gui_create_sm_crafting(struct GUI_frame* parent) {

	struct GUI_frame* sm_crafting = gui_frame_init(parent, 2);
	gui_resize(sm_crafting, parent->width - 2 * GUI_PADDING, parent->height - 2 * GUI_PADDING);
	gui_move(sm_crafting, 100, 100, 0, 0, (enum GUI_flags[]) { POS_CENTERED_X, POS_CENTERED_Y });
	gui_set_color(sm_crafting, COLOR_HEX_SEC);

	struct GUI_frame* recipe_list = gui_create_sm_recipe_list(sm_crafting, ID_recipe_list);

	int text_width = 200;

	// Recipe text
	struct GUI_frame* sm_title = gui_frame_init(sm_crafting, 0);
	gui_resize(sm_title, text_width, 40);
	gui_move(sm_title, 0, 0, GUI_PADDING, GUI_PADDING, (enum GUI_flags[]) { POS_LEFT, POS_TOP });
	gui_set_color(sm_title, COLOR_HEX_SEC);
	sm_title->text_enabled = TRUE;

	return sm_crafting;
}

// Creates recipe list block 
struct GUI_frame* gui_create_sm_recipe_list(struct GUI_frame* parent, enum GUI_ID recipe_list_id) {

	struct GUI_frame* main_frame = gui_frame_init(parent, 1);
	gui_resize(main_frame, parent->width - 2 * GUI_PADDING, parent->height - 2 * GUI_PADDING);
	gui_move(main_frame, 0, 0, 0, 0, (enum GUI_flags[]) { POS_CENTERED_X, POS_CENTERED_Y});
	gui_set_color(main_frame, COLOR_HEX_SEC);

	// Recipe item grid
	struct GUI_frame* recipes_frame = gui_create_tile_box(main_frame, GUI_RECIPE_WIDTH, GUI_RECIPE_HEIGHT, GUI_TILE_SIZE, GUI_TILE_SIZE, GUI_TILE_MARGIN, ID_recipe_frame, recipe_list_id, COLOR_HEX_SEC_2);

}

// Updates side menu crafting recipe list
void gui_update_sm_crafting(SDL_Renderer* renderer, struct MediaBin* mediaBin) {

	struct GUI_frame* recipes_list = player->gui_side_menu->children[1];
	struct GUI_frame* recipe_title = player->gui_side_menu->children[1]->children[1];
	
	// Draw text for recipe item
	SDL_FRect t_rect = {
		recipe_title->position[0],
		recipe_title->position[1],
		recipe_title->width,
		recipe_title->height
	};


	// Load recipes depending on if player is selecting a recipe for a building or is in player crafting mode
	if (player->selecting_recipe == TRUE) {

		switch (player->cursor->selected_building->type) {
			case BUILDING_CRAFTER_1: {

				gui_update_recipe_list(recipes_list, RECIPE_M_CRAFTER);
				update_text_box(renderer, recipe_title->textBox, mediaBin->font_text, &t_rect, "Select crafter recipe", COLOR_WHITE);
				
				break;
			}
			case BUILDING_BURNER_SMELTER: {

				gui_update_recipe_list(recipes_list, RECIPE_M_SMELTER);
				update_text_box(renderer, recipe_title->textBox, mediaBin->font_text, &t_rect, "Select smelter recipe", COLOR_WHITE);

				break;
			}


			
		}
	}
	else {
		gui_update_recipe_list(recipes_list, RECIPE_M_HAND);
		update_text_box(renderer, recipe_title->textBox, mediaBin->font_text, &t_rect, "Crafting recipes", COLOR_WHITE);
	}

}

// Creates building interaction side menu
struct GUI_frame* gui_create_sm_buildings(struct GUI_frame* parent) {

	// Main frame for side menu - buildings interface
	struct GUI_frame* sm_buildings = gui_frame_init(parent, 2);
	gui_resize(sm_buildings, parent->width - 2 * GUI_PADDING, parent->height - 2 * GUI_PADDING);
	gui_move(sm_buildings, 0, 0, 0, 0, (enum GUI_flags[]) { POS_CENTERED_X, POS_CENTERED_Y });
	gui_set_color(sm_buildings, COLOR_HEX_SEC);

	float container_bottom_size = 0.75;		// Size percentage of bottom container
	int inventory_margin = 60;				// Top and bottom margin for inventory tile box
	int icon_size = 40;
	int text_width = 200;
	
	int tiles_width = CRAFTER_MAX_INPUT * GUI_TILE_SIZE + (CRAFTER_MAX_INPUT - 1) * GUI_TILE_MARGIN;
	int tiles_height = 1 * GUI_TILE_SIZE + (1 - 1) * GUI_TILE_MARGIN;

	// Top Container for icon, button and text
	struct GUI_frame* sm_container_top = gui_frame_init(sm_buildings, 3);
	gui_resize(sm_container_top, sm_buildings->width - 2 * GUI_PADDING, sm_buildings->height * (1 - container_bottom_size));
	gui_move(sm_container_top, 0, 0, 0, 0, (enum GUI_flags[]) { POS_CENTERED_X, POS_TOP });
	gui_set_color(sm_container_top, COLOR_HEX_SEC);

		// Recipe icon
		struct GUI_frame* selected_recipe_icon = gui_frame_init(sm_container_top, 0);
		gui_resize(selected_recipe_icon, icon_size, icon_size);
		gui_move(selected_recipe_icon, 0, 0, 0, 0, (enum GUI_flags[]) { POS_LEFT, POS_CENTERED_Y });
		gui_set_color(selected_recipe_icon, COLOR_HEX_MAIN);

		// Recipe text
		struct GUI_frame* selected_recipe_text = gui_frame_init(sm_container_top, 0);
		gui_resize(selected_recipe_text, text_width, icon_size);
		gui_move(selected_recipe_text, 0, 0, icon_size + GUI_PADDING, 0, (enum GUI_flags[]) { POS_LEFT, POS_CENTERED_Y });
		gui_set_color(selected_recipe_text, COLOR_HEX_SEC);
		selected_recipe_text->text_enabled = TRUE;

		// Recipe change button
		struct GUI_frame* change_recipe_button = gui_frame_init(sm_container_top, 0);
		gui_resize(change_recipe_button, icon_size, icon_size);
		gui_move(change_recipe_button, 0, 0, 0, 0, (enum GUI_flags[]) { POS_RIGHT, POS_CENTERED_Y });
		gui_set_color(change_recipe_button, COLOR_HEX_GREEN);
		change_recipe_button->id = ID_sm_recipe_btn;
		gui_add_class(change_recipe_button, C_button);


	// Container for input, output and progress bar
	struct GUI_frame* sm_container_bottom = gui_frame_init(sm_buildings, 3);
	gui_resize(sm_container_bottom, sm_buildings->width, sm_buildings->height * container_bottom_size);
	gui_move(sm_container_bottom, 0, 0, 0, 0, (enum GUI_flags[]) { POS_CENTERED_X, POS_BOTTOM });
	gui_set_color(sm_container_bottom, COLOR_HEX_SEC);

		// Input frame border
		struct GUI_frame* input_frame_border = gui_frame_init(sm_container_bottom, 1);
		gui_resize(input_frame_border, tiles_width, tiles_height);
		gui_move(input_frame_border, 0, 0, 0, inventory_margin, (enum GUI_flags[]) { POS_CENTERED_X, POS_TOP });
		gui_set_color(input_frame_border, COLOR_HEX_SEC);
		input_frame_border->id = ID_sm_building_input;

		// Input frame tile box
		struct GUI_frame* input_tile_box = gui_create_tile_box(input_frame_border, CRAFTER_MAX_INPUT, 1, GUI_TILE_SIZE, GUI_TILE_SIZE, GUI_TILE_MARGIN, ID_inventory_frame, ID_sm_input_inv, COLOR_HEX_THIRD);

		// Output frame border
		struct GUI_frame* output_frame_border = gui_frame_init(sm_container_bottom, 1);
		gui_resize(output_frame_border, tiles_width, tiles_height);
		gui_move(output_frame_border, 0, 0, 0, inventory_margin, (enum GUI_flags[]) { POS_CENTERED_X, POS_BOTTOM });
		gui_set_color(output_frame_border, COLOR_HEX_SEC);
		output_frame_border->id = ID_sm_building_output;

		// Output frame tile
		struct GUI_frame* output_tile_box = gui_create_tile_box(output_frame_border, CRAFTER_MAX_INPUT, 1, GUI_TILE_SIZE, GUI_TILE_SIZE, GUI_TILE_MARGIN, ID_inventory_frame, ID_sm_output_inv, COLOR_HEX_THIRD);

		// Progress bar
		struct GUI_frame* progress_bar_border = gui_frame_init(sm_container_bottom, 1);
		gui_resize(progress_bar_border, 220, 30);
		gui_move(progress_bar_border, 0, 0, 0, 80, (enum GUI_flags[]) { POS_CENTERED_X, POS_CENTERED_Y });
		gui_set_color(progress_bar_border, COLOR_HEX_SEC);
		struct GUI_frame* progress_bar = gui_create_progress_bar(progress_bar_border, ID_sm_building_progress_bar, COLOR_HEX_THIRD, COLOR_HEX_MAIN);
	
		//gui_update_progress_bar(progress_bar, 300, 180);

	return sm_buildings;
}

// Updates side menu buildings interface with data from active building
void gui_update_sm_buildings(SDL_Renderer* renderer, struct MediaBin* mediaBin) {

	struct Building* building = player->cursor->selected_building;
	if (!building) {
		return;
	}

	struct GUI_frame* recipe_text = player->gui_side_menu->children[0]->children[0]->children[1];			// side_menu/building/top_container/recipe_text
	struct GUI_frame* recipe_icon = player->gui_side_menu->children[0]->children[0]->children[0];
	struct GUI_frame* recipe_change_btn = player->gui_side_menu->children[0]->children[0]->children[2];

	struct CraftingRecipe* selected_recipe = CraftingRecipes[building->recipe];


	// Draw text for recipe item
	SDL_FRect t_rect = {
		recipe_text->position[0],
		recipe_text->position[1],
		recipe_text->width,
		recipe_text->height
	};

	struct GUI_frame* bottom_container = player->gui_side_menu->children[0]->children[1];	// side_menu/building
	struct GUI_frame* progress_bar = bottom_container->children[2]->children[0];							// bottom_container/progress_bar

	if (building->recipe == RECIPE_NONE) {
		update_text_box(renderer, recipe_text->textBox, mediaBin->font_text, &t_rect, "Select recipe", COLOR_WHITE);

		// Reset progress bar
		gui_update_progress_bar(progress_bar, 1, 1);	// 0% progress
	}
	else {
		// Draw recipe name
		update_text_box(renderer, recipe_text->textBox, mediaBin->font_text, &t_rect, selected_recipe->title, COLOR_WHITE);


		// Update progress bar
		gui_update_progress_bar(progress_bar, selected_recipe->crafting_time, CraftingQueue[building->craft_request_id].time_left);
	}

	// Update recipe icon
	if (building->recipe != RECIPE_NONE) {

		recipe_icon->set_color = Item_data_list[selected_recipe->output_items[0].type]->color;
	}

	//set correct recipe icon

	// update input inventory
	gui_update_inventory(bottom_container->children[0], building->input_inv, renderer, mediaBin);

	// update output inventory
	gui_update_inventory(bottom_container->children[1], building->output_inv, renderer, mediaBin);

	// Show change recipe icon only for crafter and smelter
	
	if (building->type == BUILDING_CRAFTER_1 || building->type == BUILDING_BURNER_SMELTER) {
		recipe_change_btn->visibility = SHOWN;
	}
	else {
		recipe_change_btn->visibility = HIDDEN;
	}

}

// Creates a progress bar
struct GUI_frame* gui_create_progress_bar(struct GUI_frame* parent, enum GUI_ID bar_id, unsigned int base_color, unsigned int progress_color) {

	struct GUI_frame* base_frame = gui_frame_init(parent, 1);
	gui_resize(base_frame, parent->width, parent->height);
	gui_move(base_frame, 0, 0, 0, 0, (enum GUI_flags[]) { POS_CENTERED_X, POS_CENTERED_Y });
	gui_set_color(base_frame, base_color);
	base_frame->id = bar_id;

	struct GUI_frame* progress_frame = gui_frame_init(base_frame, 0);
	gui_resize(progress_frame, parent->width, parent->height);
	gui_move(progress_frame, 0, 0, 0, 0, NULL);
	gui_set_color(progress_frame, progress_color);

	return base_frame;
}

// Updates progress bar with time left
void gui_update_progress_bar(struct GUI_frame* bar, float total, float left) {

	if (!bar) return;

	// In case time left is passed as a null
	if (!left) left = total;

	float progress = total - left;
	float width_multiplier = 0;

	if (total != 0) {
		width_multiplier = progress / total;
	}

	gui_resize(bar->children[0], (int)(bar->width * width_multiplier), bar->height);
	
}


struct GUI_frame* gui_create_player_mining_bar() {

	// MAIN FRAME
	struct GUI_frame* frame = gui_frame_init(NULL, 1);
	gui_resize(frame, WINDOW_WIDTH / 2, 20);
	gui_move(frame, 0, 0, 0, 20, (enum GUI_flags[]) { POS_CENTERED_X, POS_BOTTOM });
	gui_set_color(frame, COLOR_HEX_SEC);

	// Add frame composition to list of all gui frames
	GUI_WINDOWS[gui_find_free_slot()] = frame;

	struct GUI_frame* progress_bar = gui_create_progress_bar(frame, ID_mining_bar, COLOR_HEX_THIRD, COLOR_HEX_MAIN);


	return frame;
}









// OPTIONAL, NOT IMPORTANT RN
// Automatic parent resize function
// Starts from a child and resizes parent to fit child plus padding



