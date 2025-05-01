//#include <stdio.h>			// Special system folders from links
//#include <SDL3/SDL.h>
//#include <SDL_image.h>
//
//#include "./constants.h"	// Sibling file in project folder
//#include "player.h"
//#include "game.h"
//
//SDL_Window* window = NULL;
//SDL_Renderer* renderer = NULL;
//
//int game_is_running = FALSE;
//Uint64 last_frame_time = 0;
//
//struct ball {
//	float x;
//	float y;
//	float width;
//	float height;
//	float velx;
//	float vely;
//
//} ball;
//
//int initialize_window(void) {
//
//	// Check if SDL initialization failed
//	if (!SDL_InitSubSystem(0)) {
//		fprintf(stderr, "Error initializing SDL.\n");
//		return FALSE;
//	}
//
//	// Create window
//	window = SDL_CreateWindow(
//		WINDOW_TITLE, 
//		WINDOW_WIDTH, 
//		WINDOW_HEIGHT, 
//		SDL_WINDOW_RESIZABLE       	// Flags
//	);
//
//	// Check if window failed
//	if (!window) {
//		fprintf(stderr, "Error creating SDL window.\n");
//		return FALSE;
//	}
//
//	// Create renderer
//	renderer = SDL_CreateRenderer(
//		window,		// Attached to window
//		NULL		// Default display driver
//	);
//
//	// Check if renderer failed
//	if (!renderer) {
//		fprintf(stderr, "Error creating SDL renderer.\n");
//		return FALSE;
//	}
//
//	return TRUE;
//}
//
//void process_input() {
//	
//	SDL_Event event;
//	SDL_PollEvent(&event);
//
//	switch (event.type) {
//		
//		case SDL_EVENT_QUIT:
//			game_is_running = FALSE;	// Stop game when QUIT EVENT is called
//			break;
//		
//		case SDL_EVENT_KEY_DOWN:
//			if (event.key.key == SDLK_ESCAPE) {		// Stop the game when escape key is pressed
//				game_is_running = FALSE;
//			}
//			if (event.key.key == SDLK_UP) {
//				ball.vely *= -1;
//			}
//
//			break;
//
//		
//
//	}
//}
//
//void setup() {
//	ball.x = 200;
//	ball.y = 200;
//	ball.width = 30;
//	ball.height = 30;
//	ball.velx = 300;
//	ball.vely = 50;
//}
//
//void update() {
//
//	// Logic to keep a fixed timestep:
//
//	// Sleep until we reach the frame target time
//	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - last_frame_time);
//
//	// Only call delay if we are too fast too process this frame
//	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
//		SDL_Delay(time_to_wait);
//	}
//
//	// Get delta time factor
//	float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;
//
//	last_frame_time = SDL_GetTicks();
//
//	ball.x += ball.velx * delta_time;
//	ball.y += ball.vely * delta_time;
//
//	ball.vely += GRAVITY * delta_time;
//
//	if (ball.x + ball.width > WINDOW_WIDTH || ball.x < 0) {
//		ball.velx *= -1;
//	}
//	if (ball.y + ball.height > WINDOW_HEIGHT || ball.y < 0) {
//		ball.vely *= -1;
//	}
//
//}
//
//void render() {
//
//	// Draw background
//
//	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
//	SDL_RenderClear(renderer);
//
//	// Draw game objects
//
//
//	SDL_FRect ball_rect = {
//		ball.x,
//		ball.y,
//		ball.width,
//		ball.height
//	};
//
//	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
//	SDL_RenderFillRect(renderer, &ball_rect);
//
//	SDL_RenderPresent(renderer);
//}
//
//void destroy_window() {
//
//	SDL_DestroyRenderer(renderer);
//	SDL_DestroyWindow(window);
//
//	// Clean up
//	SDL_Quit();
//}
//
//
//
//
//int main(int argc, char* args[]) {
//
//	// Init window
//	game_is_running = initialize_window();
//
//	// Setup game
//	setup();
//
//	// Game loop
//	while (game_is_running) {
//		process_input();
//		update();
//		render();
//		//SDL_Delay(FRAME_TARGET_TIME);
//	}
//
//	// Close and destroy window
//	destroy_window();
//
//	return 0;
//}
//
//
//
