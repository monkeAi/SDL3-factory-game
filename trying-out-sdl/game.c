#include <stdio.h>			// Special system folders from links
#include <SDL3/SDL.h>
#include <SDL_image.h>

#include "constants.h"	// Sibling file in project folder
#include "game.h"
#include "world.h"
#include "player.h"
#include "camera.h"
#include "inventory.h"
#include "item.h"

// Defines

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

// Global variables

int program_running = FALSE;

float delta_time = 0;
Uint64 last_frame_time = 0;

// Create an inventory with 5 slots
struct Inventory* inv = NULL;
struct Inventory* inv2 = NULL;

int initialize_window(void) {

	// Check if SDL initialization failed
	if (!SDL_InitSubSystem(0)) {
		fprintf(stderr, "Error initializing SDL.\n");
		return FALSE;
	}

	// Create window
	window = SDL_CreateWindow(
		WINDOW_TITLE,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		SDL_WINDOW_RESIZABLE       	// Flags
	);

	// Check if window failed
	if (!window) {
		fprintf(stderr, "Error creating SDL window.\n");
		return FALSE;
	}

	// Create renderer
	renderer = SDL_CreateRenderer(
		window,		// Attached to window
		NULL		// Default display driver
	);

	// Check if renderer failed
	if (!renderer) {
		fprintf(stderr, "Error creating SDL renderer.\n");
		return FALSE;
	}

	return TRUE;
}

void game_init() {
	program_running = initialize_window();

	init_tilemap();

	if (init_camera()) {
		printf("Error: Camera failed to initialize.");
	}

	init_player();

	inv = Inventory_create(5);
	inv2 = Inventory_create(3);

	// Create some items
	struct Item iron_ore = Item_create(ITEM_IRON_ORE, 100, 10); // 10 Iron Ore, max stack of 100
	struct Item iron_plate = Item_create(ITEM_IRON_PLATE, 50, 5); // 5 Iron Plates, max stack of 50

	// Add items to inventory
	if (Inventory_push_item(inv, &iron_ore) == 0) {
		printf("Iron Ore added to inventory.\n");
	}
	else {
		printf("Failed to add Iron Ore.\n");
	}

	if (Inventory_push_item(inv, &iron_plate) == 0) {
		printf("Iron Plate added to inventory.\n");
	}
	else {
		printf("Failed to add Iron Plate.\n");
	}

	Inventory_print(inv);
}

void game_loop() {

	while (program_running) {

		// Process user input
		if (game_handle_input()) {
			printf("Error: Input processing failed.");
		}

		// Update game variables and states
		if (game_update()) {
			printf("Error: Game update failed.");
		}
		
		// Render everything to the screen
		if (game_render()) {
			printf("Error: Game rendering failed.");
		}

	}

}

void game_shutdown() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	// Clean up
	SDL_Quit();
}

int game_handle_input() {

	// Reset player speed
	player->vel.x = 0;
	player->vel.y = 0;

	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type) {

	case SDL_EVENT_QUIT:
		program_running = FALSE;	// Stop game when QUIT EVENT is called
		printf("Quiting game.");
		break;

	case SDL_EVENT_KEY_DOWN:
		if (event.key.key == SDLK_ESCAPE) {		// Stop the game when escape key is pressed
			program_running = FALSE;
			printf("Escape pressed.");
		}
		if (event.key.key == SDLK_F) {

			struct Item iron_ore = Item_create(ITEM_IRON_ORE, 100, 28); // 10 Iron Ore, max stack of 100
			if (Inventory_push_item(inv, &iron_ore) == 0) {
				printf("Iron Ore added to inventory.\n");
			}
			else {
				printf("Failed to add %d Iron Ore.\n", iron_ore.quantity);
			}
			Inventory_print(inv);

		}
		if (event.key.key == SDLK_C) {
			Inventory_transfer_item(inv, inv, 2, -1);
			Inventory_print(inv);

			
		}
		if (event.key.key == SDLK_T) {

			int slot = 1;
			if (Inventory_transfer_item(inv, player->inventory, slot, -1) == 0) {
				printf("Items succesfully transfered.\n");
			}
			else {
				printf("Failed to transfer %d Items.\n", inv->slots[slot].quantity);
			}

			printf("Inventory 1:\n");
			Inventory_print(inv);
			printf("Inventory Player:\n");
			Inventory_print(player->inventory);

		}
		break;
	}

	return 0;
}

void update_deltaTime() {
	// Logic to keep a fixed timestep:

	// Sleep until we reach the frame target time
	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);

	// Only call delay if we are too fast too process this frame
	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
		SDL_Delay(time_to_wait);
	}

	// Get delta time factor
	delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;

	last_frame_time = SDL_GetTicks();
}

int game_update() {

	// Update delta time
	update_deltaTime();

	// Update tilemap
	update_tilemap();

	// Update player
	update_player(&player, delta_time);

	// Update camera positioning offset
	update_camera();

	return 0;
}

int game_render() {

	// Draw background
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	// Render elements

	render_tilemap(renderer);		// World tilemap
	render_player(renderer);		// Player

	// Render scene
	SDL_RenderPresent(renderer);

	return 0;
}