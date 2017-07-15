#ifndef TTY_H
#define TTY_H

#include <kernel/vga.h>

#define TTY_MAX_LINES 256

typedef struct tty_char
{
	uint8_t symb;
	vga_color color;
} __attribute__((packed)) tty_char;

void tty_init();
void tty_setcolor(vga_color color); // vga colors
void tty_reset_color();
void tty_puts(char* src);
void tty_putc(unsigned char a);

#define TTY_WIDTH (VGA_WIDTH/8)
#define TTY_HEIGHT (VGA_HEIGHT/16)

//#define VC_DEFAULT vga_color(VC_LIGHT_GREY,VC_BLACK)

#endif