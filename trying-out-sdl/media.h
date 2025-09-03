#ifndef MEDIA_H  
#define MEDIA_H  

#include <SDL3/SDL.h>  
#include <SDL_image.h>  
#include <SDL_ttf.h>  

// Define the MediaBin structure  
struct MediaBin {  
  TTF_Font* font_text;  
}; 

typedef struct MediaBin MediaBin;

struct MediaBin* init_media(void);  
int render_text(SDL_Renderer* renderer, TTF_Font* font, SDL_FRect* text_rect, char* text, SDL_Color color);
void free_media_bin(struct MediaBin* bin);

#endif