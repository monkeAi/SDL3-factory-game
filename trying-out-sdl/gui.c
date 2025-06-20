#include <stdio.h>	
#include "gui.h"
#include "tools.h"

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

// Resizes the provided gui frame
void gui_resize(struct GUI_frame* frame, int width, int height) {
	frame->width = width;
	frame->height = height;
}

// Moves gui frame to a new position
void gui_move(struct GUI_frame* frame, int x, int y, enum GUI_flags* FLAGS) {

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

// Returns a number of children with that class name, writes to a table of pointers to children with provided class name, it searches recursevly
int gui_find_children(struct GUI_frame* parent, enum GUI_class class_name, struct GUI_frame** matches) {

	// Amount of found mathches
	int matches_count = 0;

	// Loop through every child
	for (int i = 0; i < parent->max_children; i++) {

		// If child is NULL continue
		if (parent->children[i] == NULL) continue;

		int free_space = gui_get_first_index(matches);
		
		// If class name matches and there is still spase in the list, add pointer to end of list
		if (parent->children[i]->class == class_name && free_space != -1) {
			matches[free_space] = parent->children[i];
			matches_count++;
		}

		// Run function on the children as well
		matches_count += gui_find_children(parent->children[i], class_name, matches);
	}

	return matches_count;
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
void update_gui() {

	// Loop through all visible gui and update it
	for (int i = 0; i < MAX_GUI_WINDOWS; i++) {
		if (GUI_WINDOWS[i] == NULL) continue;
		gui_frame_update(GUI_WINDOWS[i]);
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

	int padding = 15;

	// Create all components and add them together
	struct GUI_frame* main_frame = gui_frame_init(NULL, 5);
	gui_resize(main_frame, tiles_width + 4 * padding, tiles_height + 4 * padding);
	gui_move(main_frame, 100, 100, (enum GUI_flags[]) { POS_CENTERED_X, POS_CENTERED_Y});
	gui_set_color(main_frame, 0x505050FF);
	main_frame->visibility = HIDDEN;
	main_frame->class = C_inventory;

	// Create all components and add them together
	struct GUI_frame* inv_frame_border = gui_frame_init(main_frame, 5);
	gui_resize(inv_frame_border, tiles_width + 2 * padding, tiles_height + 2 * padding);
	gui_move(inv_frame_border, 100, 100, (enum GUI_flags[]) { POS_CENTERED_X, POS_CENTERED_Y });
	gui_set_color(inv_frame_border, 0x646464FF);

	// Create a child frame for inventory tiles
	struct GUI_frame* inv_frame = gui_frame_init(inv_frame_border, GUI_INVENTORY_WIDTH * GUI_INVENTORY_HEIGHT);
	// Set size to grid of tiles inside
	gui_resize(inv_frame, tiles_width, tiles_height);
	gui_move(inv_frame, 0, 0, (enum GUI_flags[]) { POS_CENTERED_X, POS_CENTERED_Y });
	gui_set_color(inv_frame, inv_frame_border->default_color);

	// Create tiles for inventroy frame
	for (int y = 0; y < GUI_INVENTORY_HEIGHT; y++) {
		for (int x = 0; x < GUI_INVENTORY_WIDTH; x++) {

			// Create a tile
			struct GUI_frame* inv_tile = gui_frame_init(inv_frame, 0);

			// Set size 
			gui_resize(inv_tile, GUI_TILE_SIZE, GUI_TILE_SIZE);
			gui_move(inv_tile, x * GUI_TILE_SIZE + x * GUI_TILE_MARGIN, y * GUI_TILE_SIZE + y * GUI_TILE_MARGIN, NULL);
			gui_set_color(inv_tile, 0x464646FF);
			inv_tile->class = C_inventory_tile;
		}
	}



	// Add final frame composition to list of all gui frames
	GUI_WINDOWS[gui_find_free_slot()] = main_frame;

	return main_frame;
}


// Every tile has an item 
// On player side detect if its over gui then get pointer to that gui

// Creates player crafting menu
struct GUI_frame* gui_create_player_crafting() {



}

// Automatic parent resize function
// Starts from a child and resizes parent to fit child plus padding