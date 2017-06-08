/**
 * @file vga.c
 * @brief Interface to VGA text mode.
 */
#include <kernel/vga.h>
#include <kernel/io.h>
#include <kernel/memory.h>
#include <kernel/font.h>
#include <kernel/printf.h>
volatile uint16_t* vga_buffer;
static int vga_pitch;
static int vga_bpp;

int VGA_WIDTH, VGA_HEIGHT;

/**
 * @brief      Initialize VGA.
 */
void vga_init()
{
	struct vbe_info* vbm = 0x6F00;
	vga_buffer = vbm->framebuffer;
	vga_pitch = vbm->pitch;
	VGA_WIDTH = vbm->width;
	VGA_HEIGHT = vbm->height;
	vga_bpp = vbm->bpp/8;
	vga_clear();
	vga_set_cursor(0,0);
}
/**
 * @brief      Scroll one row in VGA buffer.
 */
void vga_scroll_row()
{
	for(int x = 0; x < VGA_WIDTH; x++)
	{
		for(int y = 1; y < VGA_HEIGHT; y++)
			vga_buffer[(y-1)*VGA_WIDTH+x]=vga_buffer[y*VGA_WIDTH+x];
	}
	memset(vga_buffer+(VGA_HEIGHT-1)*VGA_WIDTH,0,4*VGA_WIDTH);
	//vga_set_cursor(0,VGA_HEIGHT-2);
}


/**
 * @brief      Set cursor.
 *
 * @param[in]  row   The row of the cursor
 * @param[in]  col   The column of the cursor
 */
void vga_set_cursor(int row, int col)
{
    unsigned short position = (row * VGA_WIDTH) + col;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position&0xFF));

    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char )((position>>8)&0xFF));
}

/**
 * @brief      Puts a char to VGA buffer
 *
 * @param[in]  c      The char
 * @param[in]  color  The color
 * @param[in]  x      The row of the cursor
 * @param[in]  y      The column of the cursor
 */
void vga_putc(unsigned char c, unsigned char color, int tty_x, int tty_y)
{
	//vga_buffer[y*VGA_WIDTH+x] = vga_entry(c,color);
	for(int j = 0; j<16; j++)
	{
		uint8_t line = *(the_font+(c<<4)+j);
		for(int i = 0; i<8; i++)
			if((line >> (8-i)) & 1)
				vga_put_pixel(tty_x*8+i,tty_y*16+j,color);
	}
}

void vga_put_pixel(int x, int y, unsigned char color)
{
	uint64_t t = vga_buffer;
	volatile unsigned char* s = t;
	s += x;
	s += vga_pitch*y;
	*s = color;
}

/**
 * @brief      Clears everything in VGA buffer.
 */
void vga_clear()
{
	memset(vga_buffer,0,VGA_WIDTH*VGA_HEIGHT*vga_bpp);
}