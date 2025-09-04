#include <stdio.h>	
#include "gui.h"
#include "tools.h"
#include "inventory.h"
#include "player.h"
#include "media.h"

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

	frame->class = 0;
	frame->id = 0;

	frame->textBox = malloc(sizeof(struct TextBox));
	frame->text_enabled = FALSE;

	// Background color
	frame->default_color = 0xFFFFFFFF;
	frame->set_color = frame->default_color;

	frame->visibility = SHOWN;

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

	// Loop through every child
	for (int i = 0; i < parent->max_children; i++) {

		// If child is NULL continue
		if (parent->children[i] == NULL) continue;

		int free_space = gui_get_first_index(matches);
		
		// If class name matches and there is still space in the list, add pointer to end of list
		if (parent->children[i]->class == class_name && free_space != -1) {
			matches[free_space] = parent->children[i];
			matches_count++;
		}

		// Run function on the children as well
		matches_count += gui_find_children(parent->children[i], class_name, matches);
	}

	return matches_count;
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


	// Update GUI Player Inventory
	if (player->gui_inventory->visibility == SHOWN) {
		gui_update_inventory(player->gui_inventory, player->inventory, renderer, mediaBin);
	}

	// Update GUI Building Inventory

		// Buildings have 2 inventorys to update


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
	gui_set_color(main_frame, 0x505050FF);
	main_frame->visibility = HIDDEN;
	main_frame->class = C_inventory;

	// Create all components and add them together
	
	// INVENTORY FRAME BORDER
	struct GUI_frame* inv_frame_border = gui_frame_init(main_frame, 5);
	gui_resize(inv_frame_border, tiles_width + 2 * GUI_PADDING, tiles_height + 2 * GUI_PADDING);
	gui_move(inv_frame_border, 100, 100, GUI_PADDING, 0, (enum GUI_flags[]) { POS_LEFT, POS_CENTERED_Y });
	gui_set_color(inv_frame_border, 0x646464FF);

	// INVENTORY FRAME
	struct GUI_frame* inv_frame = gui_create_tile_box(inv_frame_border, GUI_INVENTORY_WIDTH, GUI_INVENTORY_HEIGHT, GUI_TILE_SIZE, GUI_TILE_SIZE, GUI_TILE_MARGIN, ID_inventory_frame, 0x464646FF);

	// Add final frame composition to list of all gui frames
	GUI_WINDOWS[gui_find_free_slot()] = main_frame;

	return main_frame;
}

// Creates a tile box full of frames
struct GUI_frame* gui_create_tile_box(struct GUI_frame* parent, int tiles_x, int tiles_y, int tile_w, int tile_h, int tile_margin, enum GUI_ID tiles_frame_id, unsigned int tile_color) {

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
			tile->class = C_inventory_tile;

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
	item_frame->class = C_inventory_item;
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
				gui_create_item(gui_slots->children[slot], &player->inventory->slots[slot]);

				if (!gui_slots->children[slot]->children[0]) {
					printf("Null gui item pointer");
					continue;
				}
				
				// gui_slots->children[slot]->children[0] --- Item frame adress

				// Converting integer to string for quantity
				char quantity[12];
				intToStr(game_inv->slots[slot].quantity, quantity);

				
				// Set text size and position to item
				SDL_FRect* t_rect = malloc(sizeof(SDL_FRect));

				t_rect->x = gui_slots->children[slot]->children[0]->position[0];	// x
				t_rect->y = gui_slots->children[slot]->children[0]->position[1];	// y
				t_rect->w = gui_slots->children[slot]->children[0]->width;			// Width
				t_rect->h = gui_slots->children[slot]->children[0]->height;			// Height
                
				//printf("x: %f; y: %f, w:%f, h:%f \n", t_rect->x, t_rect->y, t_rect->w, t_rect->h);

				// Update text box with correct quantity
				update_text_box(renderer, gui_slots->children[slot]->children[0]->textBox, mediaBin->font_text, t_rect, quantity, COLOR_BLACK);
			}

			// Gui inv slot has the right item
			else {


				// Update text box with correct quantity
				char quantity[12];
				intToStr(game_inv->slots[slot].quantity, quantity);


				// Set text size and position to item
				SDL_FRect* t_rect = malloc(sizeof(SDL_FRect));

				t_rect->x = gui_slots->children[slot]->children[0]->position[0];	// x
				t_rect->y = gui_slots->children[slot]->children[0]->position[1];	// y
				t_rect->w = gui_slots->children[slot]->children[0]->width;			// Width
				t_rect->h = gui_slots->children[slot]->children[0]->height;			// Height

				//printf("x: %f; y: %f, w:%f, h:%f \n", t_rect->x, t_rect->y, t_rect->w, t_rect->h);

				// Update text box with correct quantity
				update_text_box(renderer, gui_slots->children[slot]->children[0]->textBox, mediaBin->font_text, t_rect, quantity, COLOR_BLACK);
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


// Creates player's side menu gui
struct GUI_frame* gui_create_sm(struct GUI_frame* parent) {
	
	// Create side menu frame
	struct GUI_frame* side_menu = gui_frame_init(parent, 2);
	gui_resize(side_menu, GUI_SM_WIDTH - GUI_PADDING, parent->height - 2 * GUI_PADDING);
	gui_move(side_menu, 100, 100, GUI_PADDING, 0, (enum GUI_flags[]) { POS_RIGHT, POS_CENTERED_Y });
	gui_set_color(side_menu, 0x646464FF);

	// Create buildings interface
	struct GUI_frame* sm_buildings = gui_create_sm_buildings(side_menu);

	// Create crafting interface
	struct GUI_frame* sm_crafting = gui_create_sm_crafting(side_menu);


	return side_menu;
}

// Creates crafting side menu
struct GUI_frame* gui_create_sm_crafting(struct GUI_frame* parent) {

	struct GUI_frame* sm_crafting = gui_frame_init(parent, 1);
	gui_resize(sm_crafting, parent->width - 2 * GUI_PADDING, parent->height - 2 * GUI_PADDING);
	gui_move(sm_crafting, 100, 100, 0, 0, (enum GUI_flags[]) { POS_CENTERED_X, POS_CENTERED_Y });
	gui_set_color(sm_crafting, 0x235678FF);
	sm_crafting->visibility = HIDDEN;

	return sm_crafting;
}

// Creates building interaction side menu
struct GUI_frame* gui_create_sm_buildings(struct GUI_frame* parent) {

	struct GUI_frame* sm_buildings = gui_frame_init(parent, 1);
	gui_resize(sm_buildings, parent->width - 2 * GUI_PADDING, parent->height - 2 * GUI_PADDING);
	gui_move(sm_buildings, 100, 100, 0, 0, (enum GUI_flags[]) { POS_CENTERED_X, POS_CENTERED_Y });
	gui_set_color(sm_buildings, 0x464646FF);

	//sm_buildings->visibility = HIDDEN;




	
	
	return sm_buildings;
}













// OPTIONAL, NOT IMPORTANT RN
// Automatic parent resize function
// Starts from a child and resizes parent to fit child plus padding



