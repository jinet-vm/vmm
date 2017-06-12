#ifndef VGA_H
#define VGA_H

#include <stdint.h>

struct vbe_info {
   uint16_t attributes;      // deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
   uint8_t window_a;         // deprecated
   uint8_t window_b;         // deprecated
   uint16_t granularity;      // deprecated; used while calculating bank numbers
   uint16_t window_size;
   uint16_t segment_a;
   uint16_t segment_b;
   uint32_t win_func_ptr;      // deprecated; used to switch banks from protected mode without returning to real mode
   uint16_t pitch;         // number of bytes per horizontal line
   uint16_t width;         // width in pixels
   uint16_t height;         // height in pixels
   uint8_t w_char;         // unused...
   uint8_t y_char;         // ...
   uint8_t planes;
   uint8_t bpp;         // bits per pixel in this mode
   uint8_t banks;         // deprecated; total number of banks in this mode
   uint8_t memory_model;
   uint8_t bank_size;      // deprecated; size of a bank, almost always 64 KB but may be 16 KB...
   uint8_t image_pages;
   uint8_t reserved0;

   uint8_t red_mask;
   uint8_t red_position;
   uint8_t green_mask;
   uint8_t green_position;
   uint8_t blue_mask;
   uint8_t blue_position;
   uint8_t reserved_mask;
   uint8_t reserved_position;
   uint8_t direct_color_attributes;

   uint32_t framebuffer;      // physical address of the linear frame buffer; write here to draw to the screen
   uint32_t off_screen_mem_off;
   uint16_t off_screen_mem_size;   // size of memory in the framebuffer but not being displayed on the screen
   uint8_t reserved1[206];
} __attribute__ ((packed));

typedef enum vga_color {
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
} vga_color;

int VGA_WIDTH, VGA_HEIGHT; // vga_init

void vga_init();

void (*vga_put_pixel)(int x, int y, vga_color color); // vga_init
void vga_putc(unsigned char c, unsigned char color, int x, int y);

#endif