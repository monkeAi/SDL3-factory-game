#ifndef PLAYER_H
#define PLAYER_H
#include <SDL3/SDL.h>
#include "tools.h"


struct Player {
    float x_pos;
    float y_pos;
    int width;
    int height;
    struct Vector2D vel;
} Player;

extern struct Player* player;

void init_player();
void update_player(float delta_time);
void render_player(SDL_Renderer* renderer);
void handle_player_movement(float delta_time);


#endif
