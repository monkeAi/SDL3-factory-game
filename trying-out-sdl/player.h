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
    SDL_MouseButtonFlags mouse_state_before;

    int available_inventory;
    struct Inventory* inventory;
    struct GUI_frame* gui_inventory;
    struct GUI_frame* gui_side_menu;
    enum GUI_side_menu side_menu_state;
    struct PlayerCursor* cursor;
    int selecting_recipe;

    void (*toggle_inv)(struct Player* self);
};

struct PlayerCursor {
    int x_pos;
    int y_pos;
    int width;
    int height;
    unsigned int set_color;

    enum GUI_visibility visibility;

    enum CursorWatching watching_type;
    struct Building* watching_building;
    struct Building* selected_building;

    enum CursorHolding is_holding;
    struct GUI_frame* held_item;
    struct Inventory* held_item_inventory;
    int held_item_index;
    struct Inventory* watching_inventory;

    float click_cooldown;

    float deconstruct_time_left;
    struct Building* decontruct_building;
};

enum CursorWatching {
    CURSOR_NONE,
    CURSOR_GUI,
    CURSOR_BUILDING,
    CURSOR_TILE
};

enum CursorHolding {
    HOLDING_NONE,
    HOLDING_ITEM
};

extern struct Player* player;

void init_player();
void update_player(struct Player* p, float delta_time);
void render_player(SDL_Renderer* renderer);
void handle_player_movement(float delta_time);
static void handle_player_interaction(struct Player* p);
void get_mouse_update(struct Player* p);
struct PlayerCursor* player_cursor_create();
void player_cursor_update();
void render_player_cursor(SDL_Renderer* renderer);
void player_cursor_holding_reset();
static void toggle_inv(struct Player* self);


#endif
