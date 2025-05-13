#ifndef BUTTON_H
#define BUTTON_H

#include <SDL3/SDL.h>

struct GUI_frame {
	float position;
	float width, height;
	int background;
	enum GUI_state state;
};


enum GUI_state {
	CLOSED,
	OPEN
};

#endif

