#ifndef MEDIA_H  
#define MEDIA_H  

#include <SDL3/SDL.h>  
#include <SDL_image.h>  
#include <SDL_ttf.h>  

// Define the MediaBin structure  
struct MediaBin {  
	TTF_Font* font_text;  
}; 

// Combination of text texture and text rect ready for render
struct TextBox {
	SDL_Texture* texture;
	SDL_FRect* rect;
};

typedef struct MediaBin MediaBin;

struct MediaBin* init_media(void);  
int update_text_box(SDL_Renderer* renderer, struct TextBox* textBox, TTF_Font* font, SDL_FRect* text_rect, char* text, SDL_Color color);
int render_text(SDL_Renderer* renderer, TTF_Font* font, SDL_FRect* text_rect, char* text, SDL_Color color);
int render_text_box(SDL_Renderer* renderer, struct TextBox* textBox);
void free_media_bin(struct MediaBin* bin);
void free_text_box(struct TextBox* box);

#endif