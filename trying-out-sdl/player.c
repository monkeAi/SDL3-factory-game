#include <stdio.h>
#include <stdlib.h>
#include "player.h"  
#include "tools.h"  
#include "camera.h"  
#include "world.h"  
#include "constants.h"
#include "inventory.h"
#include "buildings.h"
#include "gui.h"
#include "crafting.h"

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

    // Player gui initialization
    player->gui_inventory = gui_create_player_inventory(player);
    player->gui_side_menu = gui_create_sm(player->gui_inventory);

    player->cursor = player_cursor_create();

    if (player->cursor == NULL) printf("Null cursor!\n");

    player->craft_request = NULL;

    // Methods
    player->toggle_inv = toggle_inv;
    player->selecting_recipe = FALSE;

    
}  

void update_player(struct Player *p, float delta_time) {  
   if (p == NULL) {  
       fprintf(stderr, "Player is not initialized\n");  
       return;  
   }  

   handle_player_movement(delta_time);
   player_cursor_update();
   handle_player_interaction(p);

   // Player click debounce function
   if (p->cursor->click_cooldown >= 0) p->cursor->click_cooldown -= delta_time;

   // Update deconstruction time left
   if (p->cursor->watching_building == p->cursor->decontruct_building && p->cursor->decontruct_building != NULL) {
       p->cursor->deconstruct_time_left -= delta_time;
   }
   else {
       p->cursor->deconstruct_time_left = PLAYER_DECONSTRUCT_SPEED;
       p->cursor->decontruct_building = NULL;
   }

   p->mouse_state_before = p->mouse_state;
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

    // Mouse states:
    // 0 All the time
    // 1 Left click
    // 2 Middle click
    // 4 Right click

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
    p->cursor->watching_building = NULL;

    // 1) Check if mouse is over visible gui
    for (int window = 0; window < MAX_GUI_WINDOWS; window++) {

        // if mouse cord is larger than window cord and smaller than window cord + width AND the window is visible then its inside
        if (GUI_WINDOWS[window] == NULL) continue;
        if (!GUI_WINDOWS[window]->visibility) continue;
        if (!gui_is_inside_frame(GUI_WINDOWS[window], p->mouse_pos[0], p->mouse_pos[1])) continue;

        p->cursor->watching_type = CURSOR_GUI;

        // Determine class
        switch (GUI_WINDOWS[window]->class[0]) {
        case C_NONE: {
            break;
        }

        // In case of it beeing inventory then check what inventory tile is beeing hovered
        case C_inventory: {

            //printf("Class inventory");

            // List of all inventory tile box frames to go through
            struct GUI_frame* tile_box_frames[4] = {
                player->gui_inventory->children[0],
                player->gui_side_menu->children[0]->children[1]->children[0],
                player->gui_side_menu->children[0]->children[1]->children[1],
                player->gui_side_menu->children[1]
            };

            //printf("%x %x %x \n", tile_box_frames[0], tile_box_frames[1], tile_box_frames[2]);

            // For each inventory check every tile
            for (int tile_box_frame = 0; tile_box_frame < 3; tile_box_frame++) {

                // Skip side menu building tiles if its not open
                if (player->side_menu_state == SM_CRAFTING) {
                    if (tile_box_frame == 1 || tile_box_frame == 2) continue;
                }
                else if (player->side_menu_state == SM_BUILDING && tile_box_frame == 3) continue;

                struct GUI_frame* inv_tiles[MAX_GUI_CLASS_MATHCHES] = { 0 };

                // Break if it finds no children
                if (!gui_find_children(tile_box_frames[tile_box_frame], C_inventory_tile, inv_tiles)) break;

                // Determine over witch tile it is hovering
                for (int tile = 0; tile < MAX_GUI_CLASS_MATHCHES; tile++) {

                    // Skip if null or hidden
                    if (inv_tiles[tile] == NULL) continue;
                    if (!inv_tiles[tile]->visibility) continue;
                    // Skip if mouse pos is outside of acctual tile
                    if (!gui_is_inside_frame(inv_tiles[tile], p->mouse_pos[0], p->mouse_pos[1])) continue;

                    inv_tiles[tile]->state = GUI_HOVERING;  // This is the selected tile!

                    //if (player->cursor->selected_building == NULL) continue;

                    //printf("Tile box frame %d, tile id: %d\n ", tile_box_frame, inv_tiles[tile]->id);

                    // Determine what game inventory the tile belongs to
                    switch (inv_tiles[tile]->id) {
                    case ID_sm_input_inv: { player->cursor->watching_inventory = player->cursor->selected_building->input_inv; break; }
                    case ID_sm_output_inv: { player->cursor->watching_inventory = player->cursor->selected_building->output_inv; break; }
                    case ID_player_inventory: { player->cursor->watching_inventory = player->inventory; break; }
                    default: { player->cursor->watching_inventory = NULL; break; }

                    }

                    // When left clicked on a tile
                    if (player->mouse_state == 1) {

                        // If hovered tile is clicked and it has an item inside append item position to mouse  and the tile is not recipe list
                        if (inv_tiles[tile]->id != ID_recipe_list) {

                            // It has item inside and player isn't holding anything
                            if (inv_tiles[tile]->children[0] != NULL && player->cursor->is_holding == HOLDING_NONE && player->cursor->click_cooldown <= 0) {

                                // Apend item to the mouse position
                                player->cursor->is_holding = HOLDING_ITEM;
                                player->cursor->held_item = inv_tiles[tile]->children[0];

                                // Aquire game inventory of held item
                                player->cursor->held_item_inventory = player->cursor->watching_inventory;

                                // Save held item inventory slot index
                                player->cursor->held_item_index = tile;

                                // Reset click cooldown
                                player->cursor->click_cooldown = CLICK_COOLDOWN;

                            }

                            // Place held item to hovered slot if it isnt full
                            if (inv_tiles[tile]->children[0] == NULL && player->cursor->is_holding != HOLDING_NONE) {

                                // If transfering back to itself -> let go
                                if (player->cursor->watching_inventory == player->cursor->held_item_inventory) {
                                    player_cursor_holding_reset();
                                    continue;
                                }

                                // Make the item transfer
                                int output = Inventory_transfer_item(
                                    player->cursor->held_item_inventory,
                                    player->cursor->watching_inventory,
                                    player->cursor->held_item_index,
                                    -1
                                );

                                // Reset cursor 
                                player_cursor_holding_reset();
                            }


                        }


                    }

                    player->cursor->watching_inventory = NULL;


                    // If its over:
                        // GUI inventory:
                            // clicked on -> transfer whole item if posible
                            // right clicked -> transfer only one item
                        // Ground:
                            // Item type is buildable -> open building mode -> when clicked->build and consume 1 item
                            // else if pressed z drop 1 item on ground at the tile if empty
                        // Building:
                            // not needed feature at the moment


                }
            }

            // Button functionality

            // Get all visible buttons
            // for each if clicked depending on id run their function

            struct GUI_frame* buttons[MAX_GUI_BUTTONS] = { NULL };
            gui_find_children(GUI_WINDOWS[window], C_button, buttons);


            for (int btn = 0; btn < MAX_GUI_BUTTONS; btn++) {

                if (buttons[btn] == NULL) continue;
                if (buttons[btn]->visibility == HIDDEN) continue;

                // If outside a button continue
                if (!gui_is_inside_frame(buttons[btn], p->mouse_pos[0], p->mouse_pos[1])) continue;

                // If cursor clicks on a button do stuff
                if (player->mouse_state == 1 && p->mouse_state != p->mouse_state_before) {

                    switch (buttons[btn]->id) {

                        // Crafter recipe select
                        case ID_sm_recipe_btn: {
                            
                            // Open side menu for crafting
                            player->side_menu_state = SM_CRAFTING;
                            // Set recipe select to true
                            player->selecting_recipe = TRUE;

                            //printf("Change recipe button pressed!\n");S

                            break;
                        }

                        case ID_recipe_item: {

                            struct CraftingRecipe* recipe = CraftingRecipes[buttons[btn]->set_recipe];

                            // If selecting_recipe is TRUE -> set recipe to active building and set selecting recipe to FALSE

                            if (player->selecting_recipe == TRUE) {

                                player->cursor->selected_building->recipe = recipe->name;
                                player->selecting_recipe = FALSE;
                                
                                // Open buildings side menu
                                player->side_menu_state = SM_BUILDING;

                                printf("Recipe %s set for building \n", recipe->title);
                            }
                            
                            // Else add clicked recipe to crafting queue
                            else {

                                printf("%s added to queue \n", recipe->title);
                                add_recipe_to_queue(recipe->name);

                            }

                            break;
                        }
                    }


                    // Reset mouse state before
                    p->mouse_state_before = p->mouse_state;

                }

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
            p->cursor->watching_building = Buildings[b];

            // Set cursor position to building position
            p->cursor->x_pos = Buildings[b]->coords->x;
            p->cursor->y_pos = Buildings[b]->coords->y + Buildings[b]->tile_height - 1;     // small offset for drawing

            p->cursor->width = Buildings[b]->tile_width;
            p->cursor->height = Buildings[b]->tile_height;
        }

        // If left clicks on building open player inventory and side menu for buildings
        if (p->mouse_state == 1) {

            if (p->cursor->watching_type == CURSOR_BUILDING && p->mouse_state != p->mouse_state_before) {

                // Set selected building to currently watched
                player->cursor->selected_building = player->cursor->watching_building;

                // Turn on player inventory
                player->gui_inventory->visibility = TRUE;

                // Set side menu for buildings
                // If the building has a recipe then open buildings side menu, else open select recipe
                if (player->cursor->selected_building->recipe == NULL && (player->cursor->selected_building->type == BUILDING_CRAFTER_1 || player->cursor->selected_building->type == BUILDING_BURNER_SMELTER)) {
                    player->selecting_recipe = TRUE;
                    player->side_menu_state = SM_CRAFTING;

                }
                else {

                    player->side_menu_state = SM_BUILDING;
                }

                // Reset mouse state before
                p->mouse_state_before = p->mouse_state;

            }
        }

        // If holding right click on building deconstruct it
        if (p->mouse_state == 4) {

            player->cursor->decontruct_building = player->cursor->watching_building;

            // When timer runs out and selected building is still being deconstructed -> destroy building
            if (p->cursor->deconstruct_time_left <= 0 && player->cursor->decontruct_building != NULL) {
                
                // Close side menu if active building is deconstructed
                if (p->cursor->selected_building == p->cursor->decontruct_building) {

                    player->side_menu_state = HIDDEN;
                }

                // Add building item to player inventory
                if (player->cursor->decontruct_building->building_item_type) {
                    struct Item building = Item_create(player->cursor->decontruct_building->building_item_type, 1);
                    Inventory_push_item(player->inventory, &building);
                }

                // Destroy building
                Building_destroy(player->cursor->decontruct_building);

                // Reset deconstruction timer and deconstruct_building
                player->cursor->decontruct_building = NULL;
                player->cursor->deconstruct_time_left = PLAYER_DECONSTRUCT_SPEED;

                //printf("Deconstructed building \n");

            }

        }
        
        else {

            // If right click is not held down reset timer
            player->cursor->decontruct_building = NULL;
            player->cursor->deconstruct_time_left = PLAYER_DECONSTRUCT_SPEED;
        }

    }


    // 3) Else check if mouse is over any tile
    if (p->cursor->watching_type == CURSOR_NONE) {

        // Check if selected tile index is within table size
        if (world_is_inside(selected_tile_index[0], selected_tile_index[1])) {

            if (map[selected_tile_index[1]][selected_tile_index[0]].state != TILE_FULL && map[selected_tile_index[1]][selected_tile_index[0]].type != TILE_WATER) {

                map[selected_tile_index[1]][selected_tile_index[0]].state = TILE_SELECTED;

                p->cursor->visibility = HIDDEN;
                // Set cursor position
                p->cursor->x_pos = selected_cords[0];
                p->cursor->y_pos = selected_cords[1];

                p->cursor->width = 1;
                p->cursor->height = 1;


            }

        }

        p->cursor->watching_type = CURSOR_TILE;

    }


    // Place building if left click is pressed and its not inside inv
    if (p->mouse_state == 1) {


        if (p->cursor->held_item != NULL) {

            struct Item* held_item = &p->cursor->held_item_inventory->slots[p->cursor->held_item_index];

            // Place if held item is buildable
            if (p->cursor->watching_type != CURSOR_GUI && Item_data_list[held_item->type]->is_buildable == TRUE) {

                // Create building
                if (!Building_create(Item_data_list[held_item->type]->building_type, selected_cords, DOWN)) {

                    // Remove one item from held item
                    held_item->quantity -= 1;

                    // When item is empty remove it from inventory
                    if (held_item->quantity <= 0) {

                        Inventory_remove_item(p->cursor->held_item_inventory, p->cursor->held_item_index, 1);

                        // Reset cursor 
                        player_cursor_holding_reset();
                    }


                };


            }

        }
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

    cursor->watching_building = NULL;
    cursor->selected_building = NULL;

    cursor->held_item = NULL;
    cursor->held_item_index = 0;
    cursor->is_holding = HOLDING_NONE;

    cursor->held_item_inventory = NULL;
    cursor->watching_inventory = NULL;

    cursor->click_cooldown = CLICK_COOLDOWN;
    cursor->deconstruct_time_left = PLAYER_DECONSTRUCT_SPEED;
    cursor->decontruct_building = NULL;

    return cursor;
}

void player_cursor_update() {
    player->cursor->visibility = HIDDEN;
    
}

// Release item from cursor
void player_cursor_holding_reset() {

    // Reset item gui position
    player->cursor->held_item->position[0] = player->cursor->held_item->parent->position[0];
    player->cursor->held_item->position[1] = player->cursor->held_item->parent->position[1];

    // Release item from cursor
    player->cursor->is_holding = HOLDING_NONE;
    player->cursor->held_item = NULL;

    player->cursor->held_item_inventory = NULL;

    player->cursor->held_item_index = 0;
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

static void toggle_inv(struct Player* self) {
    self->gui_inventory->visibility = !self->gui_inventory->visibility;
    self->selecting_recipe = FALSE;
}

