#include "player.h"

static int px = 64;
static int py = 64;
static const int SPEED = 4;

void init_player() {
    px = 64;
    py = 64;
}

void handle_input(SDL_Event* e) {
    if (e->type == SDL_EVENT_KEY_DOWN) {
        switch (e->key.key) {
        case SDLK_UP:    py -= SPEED; break;
        case SDLK_DOWN:  py += SPEED; break;
        case SDLK_LEFT:  px -= SPEED; break;
        case SDLK_RIGHT: px += SPEED; break;
        }
    }
}

void render_player(SDL_Renderer* renderer) {
    SDL_Rect rect = { px, py, 28, 28 };
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
}