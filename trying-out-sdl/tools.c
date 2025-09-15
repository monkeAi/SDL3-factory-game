#include "tools.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Hex2RGBA(unsigned int hex_color, unsigned int* rgba_colors) {

	rgba_colors[0] = (hex_color >> 24) & 0xFF;
	rgba_colors[1] = (hex_color >> 16) & 0xFF;
	rgba_colors[2] = (hex_color >> 8) & 0xFF;
	rgba_colors[3] = hex_color & 0xFF;

	return;
}

void intToStr(int N, char* str) {
    int i = 0;

    // Save the copy of the number for sign
    int sign = N;

    // If the number is negative, make it positive
    if (N < 0)
        N = -N;

    // Extract digits from the number and add them to the
    // string
    while (N > 0) {

        // Convert integer digit to character and store
        // it in the str
        str[i++] = N % 10 + '0';
        N /= 10;
    }

    // If the number was negative, add a minus sign to the
    // string
    if (sign < 0) {
        str[i++] = '-';
    }

    // Null-terminate the string
    str[i] = '\0';

    // Reverse the string to get the correct order
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}

void concat(char s1[], char s2[]) {

    // Go to the end of the string s1
    s1 = s1 + strlen(s1);

    // Copy characters from s2 to s1 using pointers
    while (*s1++ = *s2++);
}