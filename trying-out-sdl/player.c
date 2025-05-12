#include <stdio.h>
#include "player.h"  
#include "tools.h"  
#include "camera.h"  
#include "world.h"  
#include "constants.h"
#include "inventory.h"
#include "buildings.h"

struct Player* player;  

void init_player() {

    // Allocate memory for player
    player = malloc(sizeof(struct Player));
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
    player->available_inventory = PLAYER_START_INV_SIZE;
    player->inventory = Inventory_create(PLAYER_MAX_INV_SIZE, PLAYER_START_INV_SIZE);
}  

void update_player(struct Player *p, float delta_time) {  
   if (p == NULL) {  
       fprintf(stderr, "Player is not initialized\n");  
       return;  
   }  

   handle_player_movement(delta_time);
   handle_player_interaction(&p);
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


// Hanlde player movement
static void handle_player_movement(float delta_time) {

    const Uint8* state = SDL_GetKeyboardState(NULL);

    int x_move = 0;
    int y_move = 0;

    if (state[SDL_SCANCODE_W])      y_move = -1;
    if (state[SDL_SCANCODE_S])      y_move = 1;
    if (state[SDL_SCANCODE_A])      x_move = -1;
    if (state[SDL_SCANCODE_D])      x_move = 1;

    // Normalize speed for diagonal movement
    if (x_move != 0 && y_move != 0) {

        player->x_pos += x_move * (PLAYER_MAX_SPEED / 1.4142f) * delta_time;  // 1/sqrt(2) 
        player->y_pos += y_move * (PLAYER_MAX_SPEED / 1.4142f) * delta_time;  
    }
    else {

        player->x_pos += x_move * PLAYER_MAX_SPEED * delta_time;
        player->y_pos += y_move * PLAYER_MAX_SPEED * delta_time;
    }
}

// Get mouse position and pressed buttons
static void get_mouse_update(struct Player *p) {

    p->mouse_state = SDL_GetMouseState(&p->mouse_pos[0], &p->mouse_pos[1]);

    //printf("X:%f, Y:%f, Buttons: %u\n", p->mouse_pos[0], p->mouse_pos[1], p->mouse_state);
}

// Handle player interaction with enviroment
static void handle_player_interaction(struct Player* p) {

    // Get mouse cords
    get_mouse_update(p);

    // Determine what is selected
        // Tile
            // Check for building
        // GUI
        // Entity
    
    // Translate screen cordinates to world cordinates

    int selected_cords[2] = { 0 };

    float world_x = (p->mouse_pos[0] * -1 + world_origin_x - mainCamera->x_offset) * -1;         // x
    float world_y = (p->mouse_pos[1] * -1 + world_origin_y - mainCamera->y_offset);         // y

    selected_cords[0] = (int)floorf(world_x / TILE_SIZE);
    selected_cords[1] = (int)floorf(world_y / TILE_SIZE);

    //printf("World cords X:%d Y:%d \n", selected_cords[0], selected_cords[1]);

    // Select tile from world cordinates

    int selected_tile_index[2];

    cordinate_to_index(selected_cords, selected_tile_index);

    //printf("Index: %d, %d\n", selected_tile_index[0], selected_tile_index[1]);

    // Check if selected tile index is within table size
    if (selected_tile_index[0] >= 0 && selected_tile_index[0] < MAP_WIDTH && selected_tile_index[1] >= 0 && selected_tile_index[1] < MAP_HEIGHT) {

        map[selected_tile_index[1]][selected_tile_index[0]].state = TILE_SELECTED;
    }

    // Place building if left click is pressed
    if (p->mouse_state == 1) {
        if (Building_create(BUILDING_CRAFTER_1, selected_cords, RIGHT)) {
            printf("Could not create a new building.\n");
        }
    }


}