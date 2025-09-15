#ifndef TOOLS_H
#define TOOLS_H

struct Vector2D {
	float x;
	float y;
};


void Hex2RGBA(unsigned int hex_color, int* rgba_colors);
void intToStr(int N, char* str);
void concat(char s1[], char s2[]);

#endif