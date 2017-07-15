/**
 * @file tty.c
 * @brief Terminal.
 */


/*

how this thing works (actually, should work):


	+-----------+
	|  our own  |
	|  text     |
	|  buffer   |
	|           |
	|           |
  #=|===========|=#
  I |           | I
  I |           | I  Overton window: defined by TTY_OFFSET
  I |           | I  text from here is shown into the terminal
  #=|===========|=#
	|           |
	|           |
	+-----------+

Once bottom boundary of the owindow hits the buffer's bottom we do:
1) copy a bottom half of the buffer to the buffer
2) fix offset

*/



// buffer [0xb8000] => vesa

#include <kernel/tty.h>
#include <kernel/vga.h>
#include <kernel/memory.h>

static size_t tty_x, tty_y;
static vga_color tty_color;
static const uint64_t TTY_BUFFER = 0x7c00;
static int TTY_OFFSET = 0;
/**
 * @brief      Initialises terminal.
 */

void tty_clear();

static void tty_putsymb(uint8_t c, vga_color color, int x, int y);

void tty_init()
{
	tty_clear();
	tty_reset_color();
	for(int x = 0; x<TTY_WIDTH; x++)
		for(int y = 0; y<TTY_MAX_LINES; y++)
			tty_putsymb(0,VC_BLACK,x,y);
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
	s += TTY_WIDTH*(y+TTY_OFFSET)+x;
	vga_putc(s->symb, s->color, x, y);
}

void tty_scroll_row();

void tty_putc(uint8_t a)
{
	if(a == '\n') {tty_y++, tty_x = 0; }
	if(tty_x > TTY_WIDTH - 1) tty_x = 0, tty_y++;
	if(a != '\n')
	{
		tty_putsymb(a, tty_color, tty_x, tty_y);
		tty_refresh_sym(tty_x, tty_y - TTY_OFFSET);
        tty_x++;
	}
	if(tty_y - TTY_OFFSET > TTY_HEIGHT-1)
	{
        //vga_scroll_row(10);
        TTY_OFFSET+=10;
        tty_refresh_all();
	}
	// 	// vga_putc(a, tty_color, tty_x, tty_y);
	// 	
	// 	tty_x++;
	// }

	// if(tty_x > TTY_WIDTH)
	// {
	// 	tty_x = 0;
	// 	tty_y++;
	// }

	// if(tty_y > TTY_HEIGHT - 1)
	// {
	// 	tty_x = 0;
	// 	tty_y = TTY_HEIGHT - 1;
	// 	tty_scroll_row();
	// 	tty_clear();
	// 	tty_refresh_all();
	// }
}

void tty_scroll_row() // todo
{
	for(int x = 0; x < TTY_WIDTH; x++)
	{
		for(int y = 0; y < TTY_HEIGHT-1; y++)
		{
			tty_char *s = TTY_BUFFER;
			s += y*TTY_WIDTH+x;
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
    //return;
	for(int i = 0; i < TTY_WIDTH; i++)
		for(int j = 0; j < TTY_HEIGHT; j++)
			tty_refresh_sym(i,j);
}

void tty_reset_color()
{
	tty_color = VC_WHITE;
}