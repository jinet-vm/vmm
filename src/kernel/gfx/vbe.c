// VESA video modes interface

#include <jinet/vga.h>
#include <jinet/io.h>
#include <jinet/memory.h>
#include <jinet/font.h>
#include <jinet/printf.h>
volatile uint16_t* vbe_buffer;
static int vbe_pitch;
static int vbe_bpp;

int VBE_WIDTH, VBE_HEIGHT;

static void vbe_put_pixel_8(int x, int y, vga_color color);
static void vbe_put_pixel_24(int x, int y, vga_color color);
static void vbe_put_pixel_32(int x, int y, vga_color color);

int vbe_init(void* fb, int pitch, int width, int height, int bpp)
{
	vbe_buffer = fb;
	vbe_pitch = pitch;
	VBE_WIDTH = width;
	VBE_HEIGHT = height;
	vbe_bpp = bpp;
	if(vbe_bpp == 8)
		vbe_put_pixel = vbe_put_pixel_8;
	else if(vbe_bpp == 24)
		vbe_put_pixel = vbe_put_pixel_24;
	else if(vbe_bpp == 32)
		vbe_put_pixel = vbe_put_pixel_32;
	else return 1;
	return 0;
}

void vbe_scroll_row(int shift)
{
	uint8_t* vb = vbe_buffer;
	uint16_t gone = VBE_HEIGHT % 16 + shift*16;
	char k = vbe_bpp/8;
	for(int x = 0; x<VBE_WIDTH; x++)
		for(int y = 16*shift-16; y < VBE_HEIGHT - gone; y++)
			for(int b = 0; b < k; b++)
				vb[k*((y-16*shift)*VBE_WIDTH+x)+b] = vb[k*(y*VBE_WIDTH+x)+b];
}

void vbe_putc(unsigned char c, vga_color bg, vga_color fg, int tty_x, int tty_y)
{
	//vbe_buffer[y*VBE_WIDTH+x] = vbe_entry(c,color);
	for(int j = 0; j<16; j++)
	{
		uint8_t line = *(the_font+(c<<4)+j);
		for(int i = 0; i<8; i++)
			if((line >> (7-i)) & 1)
				vbe_put_pixel(tty_x*8+i,tty_y*16+j,fg);
			else
				vbe_put_pixel(tty_x*8+i,tty_y*16+j,bg); // todo: bg/fg
	}
}

static void vbe_put_pixel_4(int x, int y, vga_color color)
{
	uint64_t t = vbe_buffer+(VBE_WIDTH*y+x)/2;
	volatile unsigned char* s = t;
	*s = 0xff;
}

static void vbe_put_pixel_8(int x, int y, vga_color color)
{
	uint64_t t = (uint64_t)vbe_buffer+x+VBE_WIDTH*y;
	volatile uint8_t* s = t;
	*s = 0xff;
}

static void vbe_put_pixel_24(int x, int y, vga_color color)
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
	uint64_t t = (uint64_t)vbe_buffer;
	t += 3*(VBE_WIDTH*y+x);
	volatile uint32_t* s = t;
	*s = colors[color];
}

static void vbe_put_pixel_32(int x, int y, vga_color color)
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
	uint64_t t = (uint64_t)vbe_buffer+4*VBE_WIDTH*y+4*x;
	volatile uint32_t* s = t;
	*s = colors[color];
}

void vbe_fill(vga_color color)
{
	for(int x = 0; x < VBE_WIDTH; x++)
		for(int y = 0; y<VBE_HEIGHT; y++)
			vbe_put_pixel(x,y,color);
}

void vbe_clear()
{
	vbe_fill(VC_BLUE);
	//memset(vbe_buffer,0,VBE_WIDTH*VBE_HEIGHT*vbe_bpp);
}