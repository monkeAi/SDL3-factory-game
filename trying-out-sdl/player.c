#include "player.h"  
#include "tools.h"  
#include "camera.h"  
#include "world.h"  
#include "constants.h"

struct Player* player;  

void init_player() { 
    
    // Allocate memory for player
    player = malloc(sizeof(Player));  
    if (player == NULL) {  
        fprintf(stderr, "Failed to allocate memory for player\n");  
        exit(111);  
    }  

    // Initialize starting parameters
    player->x_pos = 0;  
    player->y_pos = 0;  
    player->width = 16;  
    player->height = 32;  
    player->vel.x = 0;  
    player->vel.y = 0;  
}  

void update_player(float delta_time) {  
   if (player == NULL) {  
       fprintf(stderr, "Player is not initialized\n");  
       return;  
   }  

   handle_player_movement(delta_time);
}  

void render_player(SDL_Renderer* renderer) {  
   if (player == NULL) {  
       fprintf(stderr, "Player is not initialized\n");  
       return;  
   }  

   // Create a rect for player  
   SDL_FRect player_rect = {  
       player->x_pos + world_origin_x - player->width / 2 - mainCamera->x_offset,  
       player->y_pos + world_origin_y - player->height / 2 - mainCamera->y_offset,
       player->width,  
       player->height  
   };  

   SDL_SetRenderDrawColor(renderer, 242, 147, 46, 255);  

   // Add to render  
   SDL_RenderFillRect(renderer, &player_rect);  
}


// Hanlde player movement - needs to be updated every frame
void handle_player_movement(float delta_time) {

    const Uint8* state = SDL_GetKeyboardState(NULL);

    int x_move = 0;
    int y_move = 0;

    if (state[SDL_SCANCODE_W])      y_move = -1;
    if (state[SDL_SCANCODE_S])      y_move = 1;
    if (state[SDL_SCANCODE_A])      x_move = -1;
    if (state[SDL_SCANCODE_D])      x_move = 1;

    if (x_move != 0 && y_move != 0) {
        player->x_pos += x_move * (PLAYER_MAX_SPEED / 1.4142f) * delta_time;  // 1/sqrt(2) 
        player->y_pos += y_move * (PLAYER_MAX_SPEED / 1.4142f) * delta_time;  
    }
    else {

        player->x_pos += x_move * PLAYER_MAX_SPEED * delta_time;
        player->y_pos += y_move * PLAYER_MAX_SPEED * delta_time;

    }
}