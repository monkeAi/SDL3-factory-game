#ifndef GUI_H
#define GUI_H

#include <SDL3/SDL.h>
#include "constants.h"

struct GUI_frame {
	float position[2];
	float width, height;
	int background[4];
	enum GUI_state state;
	struct GUI_frame* parent;
	int max_children;
	struct GUI_frame** children;
};


enum GUI_state {
	HIDDEN,
	SHOWN
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

extern struct GUI_frame* GUI_WINDOWS[MAX_GUI_WINDOWS];

void init_gui();
static struct GUI_frame* gui_frame_init(struct GUI_frame* parent, const int max_children);
void gui_frame_render(SDL_Renderer* renderer, struct GUI_frame* frame);
static int gui_find_free_slot();
void render_gui(SDL_Renderer* renderer);
void gui_resize(struct GUI_frame* frame, int width, int height);
void gui_move(struct GUI_frame* frame, int x, int y, enum GUI_flags* FLAGS);
void gui_set_color(struct GUI_frame* frame, int r, int g, int b, int a);
int gui_add_child(struct GUI_frame* parent, struct GUI_frame* child);

struct GUI_frame* gui_create_player_inventory();

#endif
