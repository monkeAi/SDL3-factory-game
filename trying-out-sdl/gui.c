#include <stdio.h>	
#include "gui.h"

// Main GUI frame


// inventory gui

// Building gui

// Interaction gui

// Toolbar gui

// Game Menu gui

// Table of all gui window instances
struct GUI_frame* GUI_WINDOWS[MAX_GUI_WINDOWS] = { NULL };

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

	// Background color
	gui_set_color(frame, 255, 255, 255, 255);

	frame->state = SHOWN;

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

// Sets a new background color of frame
void gui_set_color(struct GUI_frame* frame, int r, int g, int b, int a) {
	frame->background[0] = r;
	frame->background[1] = g;
	frame->background[2] = b;
	frame->background[3] = a;
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

// Renders a passed GUI frame and its children
static void gui_frame_render(SDL_Renderer* renderer, struct GUI_frame* frame) {

	// Render parent
	
	// Return if null or hidden
	if (frame == NULL || frame->state == HIDDEN) return;

	SDL_FRect frame_rect = {
		frame->position[0],
		frame->position[1],
		frame->width,
		frame->height
	};

	// Set render color
	SDL_SetRenderDrawColor(renderer, frame->background[0], frame->background[1], frame->background[2], frame->background[3]);

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

// Creates a player inventory with all needed elements
struct GUI_frame* gui_create_player_inventory() {

	// Create all components and add them together
	struct GUI_frame* main_frame = gui_frame_init(NULL, 5);
	gui_resize(main_frame, 210, 260);
	gui_move(main_frame, 100, 100, NULL);
	gui_set_color(main_frame, 80, 80, 80, 255);
	main_frame->state = HIDDEN;

	// Create all components and add them together
	struct GUI_frame* inv_frame_border = gui_frame_init(main_frame, 5);
	gui_resize(inv_frame_border, 190, 240);
	gui_move(inv_frame_border, 100, 100, (enum GUI_flags[]) { POS_CENTERED_X, POS_CENTERED_Y });
	gui_set_color(inv_frame_border, 100, 100, 100, 255);

	// Create a child frame for inventory tiles
	struct GUI_frame* inv_frame = gui_frame_init(inv_frame_border, GUI_INVENTORY_WIDTH * GUI_INVENTORY_HEIGHT);
	// Set size to grid of tiles inside
	gui_resize(inv_frame, GUI_INVENTORY_WIDTH * GUI_TILE_SIZE + (GUI_INVENTORY_WIDTH - 1) * GUI_TILE_MARGIN, GUI_INVENTORY_HEIGHT * GUI_TILE_SIZE + (GUI_INVENTORY_HEIGHT - 1) * GUI_TILE_MARGIN);
	gui_move(inv_frame, 0, 0, (enum GUI_flags[]) { POS_CENTERED_X, POS_CENTERED_Y });
	gui_set_color(inv_frame, inv_frame_border->background[0], inv_frame_border->background[1], inv_frame_border->background[2], inv_frame_border->background[3]);

	// Create tiles for inventroy frame
	for (int y = 0; y < GUI_INVENTORY_HEIGHT; y++) {
		for (int x = 0; x < GUI_INVENTORY_WIDTH; x++) {

			// Create a tile
			struct GUI_frame* inv_tile = gui_frame_init(inv_frame, 0);

			// Set size 
			gui_resize(inv_tile, GUI_TILE_SIZE, GUI_TILE_SIZE);
			gui_move(inv_tile, x * GUI_TILE_SIZE + x * GUI_TILE_MARGIN, y * GUI_TILE_SIZE + y * GUI_TILE_MARGIN, NULL);
			gui_set_color(inv_tile, 70, 70, 70, 255);

		}
	}



	// Add final frame composition to list of all gui frames
	GUI_WINDOWS[gui_find_free_slot()] = main_frame;

	return main_frame;
}

// Automatic parent resize function
// Starts from a child and resizes parent to fit child plus padding