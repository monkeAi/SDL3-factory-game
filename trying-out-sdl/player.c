#include <stdio.h>
#include "player.h"  
#include "tools.h"  
#include "camera.h"  
#include "world.h"  
#include "constants.h"
#include "inventory.h"
#include "buildings.h"
#include "gui.h"

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
    player->gui_inventory = gui_create_player_inventory();
    player->cursor = player_cursor_create();

    if (player->cursor == NULL) printf("Null cursor!\n");

    
}  

void update_player(struct Player *p, float delta_time) {  
   if (p == NULL) {  
       fprintf(stderr, "Player is not initialized\n");  
       return;  
   }  

   handle_player_movement(delta_time);
   player_cursor_update();
   handle_player_interaction(p);
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
    
    // Translate screen coordinates to world cordinates
    int selected_cords[2] = { 0 };

    float world_x = (p->mouse_pos[0] * -1 + world_origin_x - mainCamera->x_offset) * -1;         // x
    float world_y = (p->mouse_pos[1] * -1 + world_origin_y - mainCamera->y_offset);              // y

    selected_cords[0] = (int)floorf(world_x / TILE_SIZE);
    selected_cords[1] = (int)floorf(world_y / TILE_SIZE);

    //printf("World cords X:%d Y:%d \n", selected_cords[0], selected_cords[1]);

    // Translate world coordinates to tile coordinates
    int selected_tile_index[2];

    cordinate_to_index(selected_cords, selected_tile_index);

    // Get what player is looking at:

    p->cursor->watching_type = CURSOR_NONE;

    // 1) Check if mouse is over visible gui
    for (int window = 0; window < MAX_GUI_WINDOWS; window++) {

        // if mouse cord is larger than window cord and smaller than window cord + width AND the window is visible then its inside
        if (GUI_WINDOWS[window] == NULL) continue;
        if (!GUI_WINDOWS[window]->visibility) continue;
        if (!gui_is_inside_frame(GUI_WINDOWS[window], p->mouse_pos[0], p->mouse_pos[1])) continue;

        p->cursor->watching_type = CURSOR_GUI;

        // Determine class
        switch (GUI_WINDOWS[window]->class) {
        case C_NONE: {
            break;
        }

        // In case of it beeing inventory then check witch inventory tile iss hovered
        case C_inventory: {

            struct GUI_frame* inv_tiles[MAX_GUI_CLASS_MATHCHES] = { 0 };

            // Break if it finds no children
            if (!gui_find_children(GUI_WINDOWS[window], C_inventory_tile, inv_tiles)) break;

            // Determine over witch tile it is hovering
            for (int tile = 0; tile < MAX_GUI_CLASS_MATHCHES; tile++) {

                // Skip if null or hidden
                if (inv_tiles[tile] == NULL) continue;
                if (!inv_tiles[tile]->visibility) continue;
                // Skip if mouse pos is outside of acctual tile
                if (!gui_is_inside_frame(inv_tiles[tile], p->mouse_pos[0], p->mouse_pos[1])) continue;

                inv_tiles[tile]->state = GUI_HOVERING;  // This is the selected tile!

            }

            break;
        }
        }

    }


    // 2) Else check if mouse is over any building
    if (p->cursor->watching_type == CURSOR_NONE) {

        // Loop through all buildings
        for (int b = 0; b < MAX_BUILDINGS; b++) {

            // Skip if NULL or outside of window
            if (Buildings[b] == NULL) continue;

            // Check if reaching tile is inside tile world  // Possible problem 
            if (!world_is_inside(selected_tile_index[0], selected_tile_index[1])) continue;

            // Check if cursor is inside building
            if (!Building_is_inside(Buildings[b], selected_cords[0], selected_cords[1])) continue;

            p->cursor->watching_type = CURSOR_BUILDING;
            p->cursor->visibility = SHOWN;

            // Set cursor position to building position
            p->cursor->x_pos = Buildings[b]->coords->x;
            p->cursor->y_pos = Buildings[b]->coords->y + Buildings[b]->tile_height - 1;     // small offset for drawing

            p->cursor->width = Buildings[b]->tile_width;
            p->cursor->height = Buildings[b]->tile_height;
        }

    }


    // 3) Else check if mouse is over any tile
    if (p->cursor->watching_type == CURSOR_NONE) {

        // Check if selected tile index is within table size
        if (world_is_inside(selected_tile_index[0], selected_tile_index[1])) {

            if (map[selected_tile_index[1]][selected_tile_index[0]].state != TILE_FULL && map[selected_tile_index[1]][selected_tile_index[0]].type != TILE_WATER) {

                map[selected_tile_index[1]][selected_tile_index[0]].state = TILE_SELECTED;

                p->cursor->visibility = SHOWN;
                // Set cursor position
                p->cursor->x_pos = selected_cords[0];
                p->cursor->y_pos = selected_cords[1];

                p->cursor->width = 1;
                p->cursor->height = 1;


            }

        }

        p->cursor->watching_type = CURSOR_TILE;

    }


    // Place building if left click is pressed
    if (p->mouse_state == 1) {

        //printf("Mouse click world cords: X:%d Y:%d \n", selected_cords[0], selected_cords[1]);
        //Building_create(BUILDING_CRAFTER_1, selected_cords, DOWN);
    }


}


// Creates a new player cursor struct and returns its pointer
struct PlayerCursor* player_cursor_create() {

    // Allocate enough memory to the pointer
    struct PlayerCursor* cursor = (struct PlayerCursor*)malloc(sizeof(struct PlayerCursor));
    if (cursor == NULL) {
        fprintf(stderr, "Failed to allocate memory for player cursor.\n");
        exit(1);
    }

    // Init parameters
    cursor->x_pos = 0;
    cursor->y_pos = 0;
    cursor->width = 1;
    cursor->height = 1;
    cursor->set_color = 0xFFA500FF;
    cursor->watching_type = 0;
    cursor->visibility = SHOWN;

    return cursor;
}

void player_cursor_update() {
    player->cursor->visibility = HIDDEN;
}

void render_player_cursor(SDL_Renderer* renderer) {

    if (player->cursor == NULL) {
        fprintf(stderr, "Player cursor is not initialized\n");
        return;
    }

    if (player->cursor->visibility == HIDDEN) return;

    // Create a rect for player  
    SDL_FRect cursor_rect = {
        (float)player->cursor->x_pos * TILE_SIZE + world_origin_x - mainCamera->x_offset,
        (float)(player->cursor->y_pos + 1) * TILE_SIZE * -1 + world_origin_y - mainCamera->y_offset,
        player->cursor->width * TILE_SIZE,
        player->cursor->height * TILE_SIZE
    };

    // Set render color
    unsigned int rgba_colors[4] = { 0 };
    Hex2RGBA(player->cursor->set_color, rgba_colors);
    SDL_SetRenderDrawColor(renderer, rgba_colors[0], rgba_colors[1], rgba_colors[2], rgba_colors[3]);

    // Add to render  
    SDL_RenderFillRect(renderer, &cursor_rect);

}