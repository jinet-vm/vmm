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

static void vga_put_pixel_4(int x, int y, vga_color color);
static void vga_put_pixel_8(int x, int y, vga_color color);
static void vga_put_pixel_24(int x, int y, vga_color color);

void vga_init()
{
	struct vbe_info* vbm = 0x6F00;
	vga_buffer = vbm->framebuffer;
	vga_pitch = vbm->pitch;
	VGA_WIDTH = vbm->width;
	VGA_HEIGHT = vbm->height;
	vga_bpp = vbm->bpp;
	vga_clear();
	vga_set_cursor(0,0);
	if(vga_bpp == 4)
		vga_put_pixel = vga_put_pixel_4;
	else if(vga_bpp == 8)
		vga_put_pixel = vga_put_pixel_8;
	else //a tif(vga_bpp == 24)
		vga_put_pixel = vga_put_pixel_24;
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
void vga_putc(unsigned char c, vga_color color, int tty_x, int tty_y)
{
	//vga_buffer[y*VGA_WIDTH+x] = vga_entry(c,color);
	for(int j = 0; j<16; j++)
	{
		uint8_t line = *(the_font+(c<<4)+j);
		for(int i = 0; i<8; i++)
			if((line >> (8-i)) & 1)
				vga_put_pixel(tty_x*8+i,tty_y*16+j,color);
			else
				vga_put_pixel(tty_x*8+i,tty_y*16+j,VC_BLACK); // todo: bg/fg
	}
}

static void vga_put_pixel_4(int x, int y, vga_color color)
{
	uint64_t t = vga_buffer+(VGA_WIDTH*y+x)/2;
	volatile unsigned char* s = t;
	*s |= 0xff;
}

static void vga_put_pixel_8(int x, int y, vga_color color)
{
	uint64_t t = (uint64_t)vga_buffer+x+VGA_WIDTH*y;
	volatile uint8_t* s = t;
	*s = color;
}

static void vga_put_pixel_15(int x, int y, vga_color color)
{
	static uint16_t colors[16] = {
		0x00,
		0x01,
		0x02,
		0x03,
		0x04,
		0x05,
		0x14,
		0x07,
		0x38,
		0x39,
		0x3A,
		0x3B,
		0x3C,
		0x3D,
		0x3E,
		0x3F};
	uint64_t t = vga_buffer+15*VGA_WIDTH*y+15*x;
	volatile uint16_t* s = t;
	*s = color;
}

static void vga_put_pixel_24(int x, int y, vga_color color)
{
	static uint32_t colors[16] = { // colors work
		0x000000,
		0x0000aa,
		0x00aa00,
		0x00aaaa,
		0xaa0000,
		0xaa00aa,
		0xaa5500,
		0xaaaaaa,
		0x555555,
		0x5555ff,
		0x55ff55,
		0x55ffff,
		0xff5555,
		0xff55ff,
		0xffff55,
		0xffffff};
	uint64_t t = (uint64_t)vga_buffer+3*VGA_WIDTH*y+3*x;
	volatile uint32_t* s = t;
	*s |= colors[color];
}

/**
 * @brief      Clears everything in VGA buffer.
 */
void vga_clear()
{
	memset(vga_buffer,0,VGA_WIDTH*VGA_HEIGHT*vga_bpp);
}