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

	//printf("Media bin initialization complete. \n");
	// Return pointer
	return mediaBin;

}

// Load sprites


// Renders text with font and color to x,y,w,h of FRect
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

	// Posible memory leak !!!!!

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

	// Free texture
	if (text_image) {
		SDL_DestroyTexture(text_image);
		text_image = NULL;
	}
}

// Creates a text box ready for rendering
int update_text_box(SDL_Renderer* renderer, struct TextBox* textBox, TTF_Font* font, SDL_FRect* text_rect, char* text, SDL_Color color) {

	// Check for invalid data
	if (!textBox) {
		fprintf(stderr, "Error Textbox NULL.%s\n", SDL_GetError());
		return FALSE;
	}
	if (!font) {
		fprintf(stderr, "Error loading Font: %s\n", SDL_GetError());
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
	textBox->rect.x = text_rect->x;
	textBox->rect.y = text_rect->y;
	textBox->rect.w = (float)surf->w;
	textBox->rect.h = (float)surf->h;

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

	textBox->texture = text_image;
	//printf("Textbox updated\n");
}

// Renders text box
int render_text_box(SDL_Renderer* renderer, struct TextBox* textBox) {
	// Render if not NULL



	if (textBox != NULL) {
		// Crete FRect
		SDL_FRect* t_rect = malloc(sizeof(SDL_FRect));

		t_rect->x = textBox->rect.x;
		t_rect->y = textBox->rect.y;
		t_rect->w = textBox->rect.w;		
		t_rect->h = textBox->rect.h;

		// Render texture
		SDL_RenderTexture(renderer, textBox->texture, NULL, t_rect);

		//printf("Textbox rendered at: x: %f, y: %f, w: %f, h: %f\n", t_rect->x, t_rect->y, t_rect->w, t_rect->h);
		
		// Clean up
		free(t_rect);
		//SDL_DestroyTexture(textBox->texture);
	}


}

// Free media
void free_media_bin(struct MediaBin* bin) {

	if (bin->font_text) {
		TTF_CloseFont(bin->font_text);
		bin->font_text = NULL;
	}

	// Loop through image textures
}

// Free text box
void free_text_box(struct TextBox* box) {
	if (box->texture != NULL) {
		SDL_DestroyTexture(box->texture);
		box->texture = NULL;
	}
	if (box != NULL) {
		free(box);
		box = NULL;
	}
}


