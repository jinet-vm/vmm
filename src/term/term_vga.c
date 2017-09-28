// an interface to vga functions

#include <jinet/term.h>
//#include <jinet/module.h>
#include <jinet/printf.h>
#include <jinet/vmaddr.h>
#include <jinet/vga.h>
//MODULE("TERM_VGA");

#define ANSI_ARG_MAX 10
#define VGA_W 80
#define VGA_H 25

// todo: it's horrible, make vga buffer great again (not the hardcoded 0x7c00)
// todo: use term_dev properly

// allocates phys buffer for vga_init

static int x, y;
static vga_color fg, bg;

int term_vga_init(struct term_dev* t)
{
	x = y = 0;
	pg_map(VMA_TERM_FB, t->addr, 0);
	vga_init(VMA_TERM_FB);
	return 0;
}

static char ansi_stage = 0;
static char ansi_mode_a, ansi_mode_b;
static int ansi_args[ANSI_ARG_MAX];
static char ansi_args_i;

static void ansi_eval()
{
	if(ansi_mode_a == '[' && ansi_mode_b == 'm')
	{
		for(int i = 0; i<ansi_args_i; i++)
		{
			int A = ansi_args[i];
			if(30 <= A && A <= 37) // fg
				fg = A - 30;
			else if(40 <= A && A <= 47) // fg
				bg = A - 40;
			else if(A == 1) // bold
				fg = fg % 8 + 8;
			else if(A == 0)
				fg = VC_WHITE, bg = VC_BLACK;
		}
	}
}

int term_vga_putc(struct term_dev* t, char c)
{
	//tty_putc(c);
	//return 0;
	static ansi_int_b = 1;
	if(ansi_stage == 0 && c == '\e') // oh, something is happening
		ansi_stage = 1;
	else if (ansi_stage == 1 && 64 <= c && c <= 95) // starting ANSI mode
	{
		ansi_mode_a = c;
		ansi_stage = 2;
		ansi_args_i = -1;
	}
	else if (ansi_stage == 1)
	{
		return 0;
	}
	else if (ansi_stage == 2)
	{
		if(c == ';') // next!
		{
			ansi_args_i++;
			//ansi_int_b = 1;
		}
		else if('0' <= c && c <= '9') // digit
		{
			if(ansi_args_i == -1) ansi_args_i = 0;
			ansi_args[ansi_args_i] *= 10;
			ansi_args[ansi_args_i] += (c-'0');
		}
		else if(64 <= c && c <= 126) // ANSI end
		{
			ansi_mode_b = c;
			ansi_args_i++;
			ansi_stage = 0;
			ansi_eval();
			for(int i = 0; i<ansi_args_i; i++)
				ansi_args[i] = 0;
			ansi_mode_b = ansi_mode_a = 0;
			ansi_args_i = -1;
			//ansi_int_b = 1;
		}
		// else: ignore
	}
	else if(ansi_stage == 0)
	{
		if(c != '\n')
			vga_putc(c, bg, fg, x, y);
		if(c == '\n')
		{
			x = 0;
			y++;
		}
		else
			x++;
		if(x >= 80)
			x = 0, y++;
		if(y >= VGA_H)
		{
			x=0;
			y = VGA_H - 1;
			vga_scroll_row(1);
		}
	}

	return 0;
}