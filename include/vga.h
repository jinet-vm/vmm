#ifndef VGA_H
#define VGA_H

#include <stdint.h>

enum vga_color {
	VC_BLACK = 0,
	VC_BLUE = 1,
	VC_GREEN = 2,
	VC_CYAN = 3,
	VC_RED = 4,
	VC_MAGENTA = 5,
	VC_BROWN = 6,
	VC_LIGHT_GREY = 7,
	VC_DARK_GREY = 8,
	VC_LIGHT_BLUE = 9,
	VC_LIGHT_GREEN = 10,
	VC_LIGHT_CYAN = 11,
	VC_LIGHT_RED = 12,
	VC_LIGHT_MAGENTA = 13,
	VC_LIGHT_BROWN = 14,
	VC_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

#endif