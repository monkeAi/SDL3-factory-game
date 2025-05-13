#ifndef BUTTON_H
#define BUTTON_H

#include <SDL3/SDL.h>
#include "gui.h"

struct GUI_button {
	float position;
	float width, height;
	int background;
	enum GUI_state state;
	struct gui* parent;
};



#endif
