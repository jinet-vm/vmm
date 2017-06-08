/**
 * @file tty.c
 * @brief Terminal.
 */

#include <kernel/tty.h>
#include <kernel/vga.h>
#include <kernel/memory.h>

static size_t tty_x, tty_y;
static uint8_t tty_color;

/**
 * @brief      Initialises terminal.
 */
void tty_init()
{
	tty_x = 0;
	tty_y = 0;
	tty_color = VC_DEFAULT;
}

/**
 * @brief      Sets current color of terminal.
 *
 * @param[in]  color  The color
 */
void tty_setcolor(char color)
{
	tty_color = color;
}

/**
 * @brief      Puts a character on terminal.
 *
 * @param[in]  a     The character.
 */
// void tty_putc(char a)
// {
// 	if(a == '\n') tty_y++, tty_x = 0;
// 	else
// 	{
// 		vga_putc(a, tty_color, tty_x, tty_y);
// 		tty_x++;
// 	}

// 	if(tty_x > VGA_WIDTH)
// 	{
// 		tty_x = 0;
// 		tty_y++;
// 	}

// 	if(tty_y > VGA_HEIGHT - 1)
// 	{
// 		tty_x = 0;
// 		tty_y = VGA_HEIGHT - 1;
// 		vga_scroll_row();
// 	}
// }

unsigned char c = 0;

void tty_putc(unsigned char a)
{
	if(a == '\n') tty_y++, tty_x = 0;
	else
	{
		// struct vga_pixel white;
		// white.r=white.g=white.b=0xff;
		vga_putc(a, 100+(c++) % 25, tty_x, tty_y);
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


/**
 * @brief      Puts a null-terminated string on terminal
 *
 * @param      src   The string.
 */
void tty_puts(char* src)
{
	volatile char* c = src;
	while(*c != 0)
	{
		tty_putc(*c);
		*c++;
	}
}