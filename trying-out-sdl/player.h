#ifndef PLAYER_H
#define PLAYER_H
#include <SDL3/SDL.h>

void init_player();
void handle_input(SDL_Event* e);
void render_player(SDL_Renderer* renderer);

#endif
