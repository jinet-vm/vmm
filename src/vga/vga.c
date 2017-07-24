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

//static void vga_put_pixel_4(int x, int y, vga_color color);
static void vga_put_pixel_8(int x, int y, vga_color color);
static void vga_put_pixel_24(int x, int y, vga_color color);
static void vga_put_pixel_32(int x, int y, vga_color color);

void vga_init()
{
	struct vbe_info* vbm = 0x6F00;
	vga_buffer = vbm->framebuffer;
	vga_pitch = vbm->pitch;
	VGA_WIDTH = vbm->width;
	VGA_HEIGHT = vbm->height;
	//VGA_HEIGHT -= VGA_HEIGHT % 16;
	vga_bpp = vbm->bpp;
	unsigned volatile int *s = (uint64_t)vga_buffer;
	//vga_clear();
	//vga_set_cursor(0,0);
	if(vga_bpp == 8)
		vga_put_pixel = vga_put_pixel_8;
	else if(vga_bpp == 24)
		vga_put_pixel = vga_put_pixel_24;
	else // if(vga_bpp == 32)
		vga_put_pixel = vga_put_pixel_32;
	//vga_put_pixel = vga_put_pixel_8;
	//vga_put_pixel = vga_put_pixel_32;
}

void vga_scroll_row(int shift)
{
	uint8_t* vb = vga_buffer;
	uint16_t gone = VGA_HEIGHT % 16 + shift*16;
	char k = vga_bpp/8;
	for(int x = 0; x<VGA_WIDTH; x++)
		for(int y = 16*shift-16; y < VGA_HEIGHT - gone; y++)
			for(int b = 0; b < k; b++)
				vb[k*((y-16*shift)*VGA_WIDTH+x)+b] = vb[k*(y*VGA_WIDTH+x)+b];
	// for(int x = 0; x < VGA_WIDTH; x++)
	// {
	// 	for(int y = 1; y < VGA_HEIGHT; y++)
	// 		vga_buffer[(y-1)*VGA_WIDTH+x]=vga_buffer[y*VGA_WIDTH+x];
	// }
	//memset(vb+k*(VGA_HEIGHT-gone)*VGA_WIDTH,0,k*gone*VGA_WIDTH);
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
void vga_putc(unsigned char c, vga_color bg, vga_color fg, int tty_x, int tty_y)
{
	//vga_buffer[y*VGA_WIDTH+x] = vga_entry(c,color);
	for(int j = 0; j<16; j++)
	{
		uint8_t line = *(the_font+(c<<4)+j);
		for(int i = 0; i<8; i++)
			if((line >> (7-i)) & 1)
				vga_put_pixel(tty_x*8+i,tty_y*16+j,fg);
			else
				vga_put_pixel(tty_x*8+i,tty_y*16+j,bg); // todo: bg/fg
	}
}

static void vga_put_pixel_4(int x, int y, vga_color color)
{
	uint64_t t = vga_buffer+(VGA_WIDTH*y+x)/2;
	volatile unsigned char* s = t;
	*s = 0xff;
}
static void vga_put_pixel_8(int x, int y, vga_color color)
{
	uint64_t t = (uint64_t)vga_buffer+x+VGA_WIDTH*y;
	volatile uint8_t* s = t;
	*s = 0xff;
}
static void vga_put_pixel_24(int x, int y, vga_color color)
{
	static uint32_t colors[16] = { // colors work
		0x000000,
		0x800000,
		0x008000,
		0x808000,
		0x000080,
		0x800080,
		0x008080,
		0xC0C0C0,
		0x808080,
		0xff0000,
		0x00ff00,
		0xffff00,
		0x0000ff,
		0xff00ff,
		0x00ffff,
		0xffffff};
	uint64_t t = (uint64_t)vga_buffer;
	t += 3*(VGA_WIDTH*y+x);
	volatile uint32_t* s = t;
	*s = colors[color];
}

static void vga_put_pixel_32(int x, int y, vga_color color)
{
	static uint32_t colors[16] = { // colors work
		0x000000,
		0x800000,
		0x008000,
		0x808000,
		0x000080,
		0x800080,
		0x008080,
		0xC0C0C0,
		0x808080,
		0xff0000,
		0x00ff00,
		0xffff00,
		0x0000ff,
		0xff00ff,
		0x00ffff,
		0xffffff};
	uint64_t t = (uint64_t)vga_buffer+4*VGA_WIDTH*y+4*x;
	volatile uint32_t* s = t;
	*s = colors[color];
}

void vga_fill(vga_color color)
{
	for(int x = 0; x < VGA_WIDTH; x++)
		for(int y = 0; y<VGA_HEIGHT; y++)
			vga_put_pixel(x,y,color);
}
void vga_clear()
{
	vga_fill(VC_BLUE);
	//memset(vga_buffer,0,VGA_WIDTH*VGA_HEIGHT*vga_bpp);
}