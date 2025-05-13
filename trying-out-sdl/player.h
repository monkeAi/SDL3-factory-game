#ifndef PLAYER_H
#define PLAYER_H
#include <SDL3/SDL.h>
#include "tools.h"
#include "gui.h"


struct Player {
    float x_pos;
    float y_pos;
    int width;
    int height;
    struct Vector2D vel;
    float mouse_pos[2];
    SDL_MouseButtonFlags mouse_state;
    int available_inventory;
    struct Inventory* inventory;
    struct GUI_frame* gui_inventory;
};

extern struct Player* player;

void init_player();
void update_player(struct Player* p, float delta_time);
void render_player(SDL_Renderer* renderer);
void handle_player_movement(float delta_time);
static void handle_player_interaction(struct Player* p);
void get_mouse_update(struct Player* p);


#endif
