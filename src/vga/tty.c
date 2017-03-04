#include <kernel/tty.h>
#include <kernel/vga.h>
#include <kernel/memory.h>

uint8_t tty_color;

size_t tty_x, tty_y;
uint8_t tty_color;

void tty_init()
{
	tty_x = 0;
	tty_y = 0;
	tty_color = vga_color(VC_LIGHT_GREY,VC_BLACK);
}

void tty_setcolor(uint8_t color)
{
	tty_color = color;
}

void tty_putc(char a)
{
	if(a == '\n') tty_y++, tty_x = 0;
	else
	{
		vga_putc(a, tty_color, tty_x, tty_y);
		tty_x++;
	}

	if(tty_x > VGA_WIDTH)
	{
		tty_x = 0;
		tty_y++;
	}

	if(tty_y > VGA_HEIGHT - 1)
	{
		tty_x = 0;
		tty_y = VGA_HEIGHT - 1;
		vga_scroll_row();
	}
}

void tty_puts(char* src)
{
	volatile char* c = src;
	while(*c != 0)
	{
		tty_putc(*c);
		*c++;
	}
}