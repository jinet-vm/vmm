/**
 * @file vga.c
 * @brief Interface to VGA text mode.
 */
#include <jinet/vga.h>
#include <jinet/io.h>
#include <jinet/memory.h>
#include <jinet/font.h>
#include <jinet/printf.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

uint16_t* VGA_BUFFER;

void vga_init(void* buffer)
{
	VGA_BUFFER = buffer;
}

void vga_scroll_row(int shift)
{
	uint16_t gone = VGA_HEIGHT % 16 + shift*16;
	for(int x = 0; x < VGA_WIDTH; x++)
	{
		for(int y = 1; y < VGA_HEIGHT; y++)
			VGA_BUFFER[(y-1)*VGA_WIDTH+x]=VGA_BUFFER[y*VGA_WIDTH+x];
	}
	memset((uint64_t)VGA_BUFFER+2*(VGA_HEIGHT-gone)*VGA_WIDTH,0,2*gone*VGA_WIDTH);
}

void vga_set_cursor(int row, int col)
{
    unsigned short position = (row * VGA_WIDTH) + col;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position&0xFF));

    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char )((position>>8)&0xFF));
}

static uint8_t ansi_to_vga(vga_color c)
{
	uint8_t out[] = 
	{
		0x0, 0x4, 0x2, 0x6, 0x1, 0x5, 0x3, 0x7, 
		0x8, 0xC, 0xA, 0xE, 0x9, 0xD, 0xB, 0xF
	};
	return out[c & 0xF];
}

void vga_putc(unsigned char c, vga_color bg, vga_color fg, int tty_x, int tty_y)
{
	uint16_t *t = VGA_BUFFER;
	t += tty_y*VGA_HEIGHT + tty_x;
	fg = ansi_to_vga(fg);
	bg = ansi_to_vga(bg);
	*t = c | (fg << 8) | (bg << 12);
}

void vga_clear()
{
	//vga_fill(VC_BLUE);
	//memset(vga_buffer,0,VGA_WIDTH*VGA_HEIGHT*vga_bpp);
}