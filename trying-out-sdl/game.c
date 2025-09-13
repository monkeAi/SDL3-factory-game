#include <stdio.h>			
#include <SDL3/SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "constants.h"	
#include "game.h"
#include "media.h"
#include "world.h"
#include "player.h"
#include "camera.h"
#include "inventory.h"
#include "item.h"
#include "recipes.h"
#include "crafting.h"
#include "buildings.h"
#include "OreManager.h"
 
// Defines

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
MediaBin* game_media = NULL;


// TODO

// Game as a struct with render, media and states -> other code has access to it

// Global variables

int program_running = FALSE;

float delta_time = 0;
Uint64 last_frame_time = 0;

int initialize_window(void) {

	SDL_Color color = { 255, 255, 255, SDL_ALPHA_OPAQUE };

	// Init SDL
	if (!SDL_InitSubSystem(0)) {
		fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
		return FALSE;
	}

	// Init TTF
	if (!TTF_Init()) {
		fprintf(stderr, "Error initializing SDL3_ttf: %s\n", SDL_GetError());
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

	game_media = init_media();

	initOreManager();

	init_tilemap();

	if (init_camera()) {
		printf("Error: Camera failed to initialize.");
	}

	init_player();

	init_item_data_list();

	init_recipes();

	init_gui();

	int selected_cords[2] = { 0 };
	int selected_cords2[2] = { 10, 8 };
	int selected_cords3[2] = { -4, - 15 };
	Building_create(BUILDING_CRAFTER_1, selected_cords, RIGHT);
	Building_create(BUILDING_CRAFTER_1, selected_cords2, RIGHT);
	//Building_create(BUILDING_CRAFTER_1, selected_cords3, LEFT);

	struct Item crafter = Item_create(ITEM_CRAFTER_1, 3);
	Inventory_push_item(player->inventory, &crafter);

	struct Item miner = Item_create(ITEM_BURNER_MINER, 3);
	Inventory_push_item(player->inventory, &miner);
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

	// Free fonts and images
	free_media_bin(game_media);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	// Clean up
	TTF_Quit();
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
		}
		if (event.key.key == SDLK_F) {

			struct Item iron_plate = Item_create(ITEM_IRON_PLATE, 100);
			if (Inventory_push_item(player->inventory, &iron_plate) == 0) {
				printf("%d Iron Plates added to inventory.\n", iron_plate.quantity);
			}
			else {
				printf("Failed to add %d Iron Ore.\n", iron_plate.quantity);
			}

		}
		if (event.key.key == SDLK_C) {

			craft_item(player->inventory, player->inventory, RECIPE_IRON_GEAR);
			
		}
		if (event.key.key == SDLK_E) {

			//printf("Inventory Player:\n");
			//Inventory_print(player->inventory);

			// Toggle inventory visibility
			player->toggle_inv(player);
			player->side_menu_state = SM_CRAFTING;


		}
		if(event.key.key == SDLK_B) {

			player->side_menu_state = SM_BUILDING;

		}
		if (event.key.key == SDLK_X) {
	
			/*int coords[2] = { 0, 0 };
			if (Building_create(BUILDING_CRAFTER_1, coords, RIGHT)) {
				printf("Could not create a new building.\n");
			}*/

			craft_item(Buildings[0]->input_inv, Buildings[0]->output_inv, RECIPE_IRON_GEAR);

		}
		if (event.key.key == SDLK_Z) {

			Inventory_transfer_item(player->inventory, Buildings[0]->input_inv, 0, 30);
		}
		if (event.key.key == SDLK_L) {

			player->cursor->selected_building = Buildings[1];
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
	update_player(player, delta_time);

	// Update camera positioning offset
	update_camera();

	// Update every building
	update_buildings();

	// Update crafting queue
	update_craft_queue(delta_time);

	// Update all gui
	update_gui(renderer, game_media);


	return 0;
}

int game_render() {

	// Draw background
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	// Render elements
	render_tilemap(renderer);		// World tilemap

	render_ores(renderer);
	render_buildings(renderer);

	render_player_cursor(renderer);
	render_player(renderer);		// Player


	// Render GUI
	render_gui(renderer);

	
	// Text rendering example
	SDL_FRect t_rect = {
		200,	// x
		100,	// y
		300,	// Width
		100		// Height
	};
	render_text(renderer, game_media->font_text, &t_rect, "Hello", COLOR_WHITE);



	// Text rendering example with textbox
	//SDL_FRect t_rect2 = {
	//	200,	// x
	//	100,	// y
	//	300,	// Width
	//	100		// Height
	//};
	//struct TextBox* tb = malloc(sizeof(struct TextBox));;
	//update_text_box(renderer, tb, game_media->font_text, &t_rect2, "30", COLOR_BLACK);
	//render_text_box(renderer, tb);



	// Render scene
	SDL_RenderPresent(renderer);

	return 0;
}



/*

- load media
	- loads all images and fonts and stores pointers in a list
- render text function
	- inputs string and size 
	- renders text texture

- each tile, player, building has texture and frect in their struct
	- at start load images into textures.


*/