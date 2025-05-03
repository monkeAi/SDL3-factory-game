#include <stdio.h>			// Special system folders from links
#include <SDL3/SDL.h>
#include <SDL_image.h>

#include "./constants.h"	// Sibling file in project folder
#include "game.h"
#include "world.h"
#include "player.h"
#include "camera.h"

// Defines

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

// Global variables

int program_running = FALSE;

float delta_time = 0;
Uint64 last_frame_time = 0;

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

	int cord[2] = {0, 0};
	int output[2];

	cordinate_to_index(&cord, &output);

	printf("%d, %d\n", output[0], output[1]);

	init_tilemap();

	if (init_camera()) {
		printf("Error: Camera failed to initialize.");
	}


	init_player();

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