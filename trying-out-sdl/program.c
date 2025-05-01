#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>
#include <stdbool.h>

#define SDL_FLAGS SDL_INIT_VIDEO

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;

bool game_init_sdl();

bool game_init_sdl() {
	if (!SDL_Init(SDL_FLAGS)) {
		fprintf(stderr, "Error initializing SDL3: %s\n", SDL_GetError());
		return false;
	}

	return true;
}

void game_free() {
	SDL_Quit();
}

