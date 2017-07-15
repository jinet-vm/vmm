/*

how this thing works (actually, should work YET):


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

// todo: rename this file
// todo: make ansi escape sequences great again

#include <kernel/tty.h>
#include <kernel/vga.h>
#include <kernel/memory.h>

static size_t tty_x, tty_y;
static vga_color tty_color;
static const uint64_t TTY_BUFFER = 0x7c00;
static int TTY_OFFSET = 0;


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

static void tty_putsymb(uint8_t c, vga_color color, int x, int y)
{
	volatile tty_char* s = (uint64_t)TTY_BUFFER;
	s += TTY_WIDTH*y+x;
	s->symb = c;
	s->color = color;
}

void tty_refresh_sym(int x, int y)
{
	volatile tty_char *s = (uint64_t)TTY_BUFFER;
	s += TTY_WIDTH*(y+TTY_OFFSET)+x;
	vga_putc(s->symb, s->color, x, y);
}


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
}

void tty_reset_color()
{
	tty_color = VC_WHITE;
}

void tty_refresh_all()
{
    //return;
    for(int i = 0; i < TTY_WIDTH; i++)
        for(int j = 0; j < TTY_HEIGHT; j++)
            tty_refresh_sym(i,j);
}