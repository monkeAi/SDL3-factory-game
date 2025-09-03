#include <SDL_ttf.h>
#include <SDL3/SDL.h>
#include <SDL_image.h>
#include "constants.h"
#include "media.h"
#include <stdlib.h>


// Media bin

struct MediaBin* init_media(void) {

	// Allocate enough memory to the pointer
	struct MediaBin* mediaBin = (struct MediaBin*)malloc(sizeof(struct MediaBin));
	if (mediaBin == NULL) {
		fprintf(stderr, "Failed to allocate memory for media.\n");
		exit(1);
	}

	// Load text font
	mediaBin->font_text = TTF_OpenFont("fonts/Roboto.ttf", TEXT_SIZE);
	if (!mediaBin->font_text) {
		fprintf(stderr, "Error Opening Font: %s\n", SDL_GetError());
		exit(1);
	}

	printf("Media bin init complete.");
	// Return pointer
	return mediaBin;

}

// Load sprites

// Load fonts


// Render text with font and color to x,y,w,h of FRect
int render_text(SDL_Renderer* renderer, TTF_Font* font, SDL_FRect* text_rect, char* text, SDL_Color color) {

	// Check for invalid data
	if (!font) {
		fprintf(stderr, "Error loading Font: %s\n", SDL_GetError());
		return FALSE;
	}
	if (!text_rect) {
		fprintf(stderr, "Error loading rect: %s\n", SDL_GetError());
		return FALSE;
	}
	if (!text) {
		fprintf(stderr, "Error loading text: %s\n", SDL_GetError());
		return FALSE;
	}


	// Create surface for text
	SDL_Surface* surf = TTF_RenderText_Blended(font, text, 0, color);

	if (!surf) {
		fprintf(stderr, "Error rendering text to Surface: %s\n", SDL_GetError());
		return FALSE;
	}

	// Set width and height of surface to rect
	text_rect->w = (float)surf->w;
	text_rect->h = (float)surf->h;

	// Create texture from surface
	SDL_Texture* text_image = SDL_CreateTextureFromSurface(renderer, surf);

	// Destroy surface
	SDL_DestroySurface(surf);
	surf = NULL;
	if (!text_image) {
		fprintf(stderr, "Error creating Texture from Surface: %s\n",
			SDL_GetError());
		return FALSE;
	}

	// Render texture
	SDL_RenderTexture(renderer, text_image, NULL, text_rect);
}


