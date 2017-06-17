/**
 * @file tty.c
 * @brief Terminal.
 */

// buffer [0xb8000] => vesa

#include <kernel/tty.h>
#include <kernel/vga.h>
#include <kernel/memory.h>

static size_t tty_x, tty_y;
static vga_color tty_color;
static const uint64_t TTY_BUFFER = 0x7c00;
/**
 * @brief      Initialises terminal.
 */

void tty_clear();

void tty_init()
{
	tty_clear();
	tty_reset_color();
}

void tty_clear()
{
	tty_x = 0;
	tty_y = 0;
	memset(TTY_BUFFER,0,sizeof(tty_char)*TTY_WIDTH*TTY_HEIGHT);
}

/**
 * @brief      Sets current color of terminal.
 *
 * @param[in]  color  The color
 */
void tty_setcolor(vga_color color)
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

static void tty_putsymb(uint8_t c, vga_color color, int x, int y)
{
	volatile tty_char* s = (uint64_t)TTY_BUFFER;
	s += TTY_WIDTH*y+x;
	s->symb = c;
	s->color = color;
	//vga_putc(s->symb, s->color, x, y);
	//tty_refresh_sym(x,y);
}

void tty_refresh_sym(int x, int y)
{
	volatile tty_char *s = (uint64_t)TTY_BUFFER;
	s += TTY_WIDTH*y+x;
	vga_putc(s->symb, s->color, x, y);
}

void tty_scroll_row();

void tty_putc(uint8_t a)
{
	if(a == '\n') tty_y++, tty_x = 0;
	else
	{
		tty_putsymb(a, tty_color, tty_x, tty_y);
		// vga_putc(a, tty_color, tty_x, tty_y);
		tty_refresh_sym(tty_x, tty_y);
		tty_x++;
	}

	if(tty_x > TTY_WIDTH)
	{
		tty_x = 0;
		tty_y++;
	}

	if(tty_y > TTY_HEIGHT - 1)
	{
		tty_x = 0;
		tty_y = VGA_HEIGHT - 1;
		//tty_scroll_row();
		tty_refresh_all();
	}
	//tty_refresh_all();
}

void tty_scroll_row()
{
	for(int x = 0; x < TTY_WIDTH; x++)
	{
		for(int y = 1; y < TTY_HEIGHT; y++)
		{
			tty_char *s = TTY_BUFFER;
			s += (y-1)*TTY_WIDTH+x;
			tty_char *k = s;
			s += TTY_WIDTH;
			*s = *k;
		}
	}
	memset(TTY_BUFFER+sizeof(tty_char)*(VGA_HEIGHT-1)*VGA_WIDTH,0,sizeof(tty_char)*VGA_WIDTH);
	//vga_set_cursor(0,VGA_HEIGHT-2);
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

void tty_refresh_all()
{
	for(int i = 0; i < TTY_WIDTH; i++)
		for(int j = 0; j < TTY_HEIGHT; j++)
			tty_refresh_sym(i,j);
}

void tty_reset_color()
{
	tty_color = VC_WHITE;
}