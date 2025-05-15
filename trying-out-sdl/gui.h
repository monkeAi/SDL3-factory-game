#ifndef GUI_H
#define GUI_H

#include <SDL3/SDL.h>
#include "constants.h"
#include "item.h"
#include "inventory.h"

struct GUI_frame {
	float position[2];
	float width, height;
	unsigned int set_color;
	unsigned int default_color;
	enum GUI_visibility visibility;
	enum GUI_state state;
	struct GUI_frame* parent;
	int max_children;
	struct GUI_frame** children;
	enum GUI_class class;

};


enum GUI_visibility {
	HIDDEN,
	SHOWN
};

enum GUI_state {
	GUI_NONE,
	GUI_HOVERING,
	GUI_SELECTED,
};

enum GUI_flags {
	POS_NONE,
	POS_CENTERED_X,
	POS_CENTERED_Y,
	POS_TOP,
	POS_BOTTOM,
	POS_LEFT,
	POS_RIGHT
};

enum GUI_class {
	C_NONE,
	C_inventory,
	C_inventory_tile,
};

extern struct GUI_frame* GUI_WINDOWS[MAX_GUI_WINDOWS];

void init_gui();
void update_gui();
static struct GUI_frame* gui_frame_init(struct GUI_frame* parent, const int max_children);
void gui_frame_render(SDL_Renderer* renderer, struct GUI_frame* frame);
static int gui_find_free_slot();
void render_gui(SDL_Renderer* renderer);
void gui_resize(struct GUI_frame* frame, int width, int height);
void gui_move(struct GUI_frame* frame, int x, int y, enum GUI_flags* FLAGS);
void gui_set_color(struct GUI_frame* frame, unsigned int new_color);
void gui_reset_color(struct GUI_frame* frame);
int gui_add_child(struct GUI_frame* parent, struct GUI_frame* child);
int gui_get_first_index(struct GUI_frame** matches);
int gui_find_children(struct GUI_frame* parent, enum GUI_class class_name, struct GUI_frame** matches);
void gui_frame_update(struct GUI_frame* frame);
int gui_is_inside_frame(struct GUI_frame* frame, int x, int y);

struct GUI_frame* gui_create_player_inventory();

#endif
