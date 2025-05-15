#include "tools.h"
#include <stdio.h>
#include <stdlib.h>

void Hex2RGBA(unsigned int hex_color, unsigned int* rgba_colors) {

	rgba_colors[0] = (hex_color >> 24) & 0xFF;
	rgba_colors[1] = (hex_color >> 16) & 0xFF;
	rgba_colors[2] = (hex_color >> 8) & 0xFF;
	rgba_colors[3] = hex_color & 0xFF;

	return;
}