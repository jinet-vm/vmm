// an interface to vga functions

#include <kernel/term.h>
//#include <kernel/module.h>
#include <kernel/printf.h>
//MODULE("TERM_VGA");

#define ANSI_ARG_MAX 10

// todo: it's horrible, make vga buffer great again (not the hardcoded 0x7c00)
// todo: use term_dev properly

int term_vga_init(struct term_dev* t)
{
	vga_init();
	tty_init();
	return 0;
}

static char ansi_stage = 0;
static char ansi_mode_a, ansi_mode_b;
static int ansi_args[ANSI_ARG_MAX];
static char ansi_args_i;

static void ansi_eval()
{
	// printf("%c ... %c\n", ansi_mode_a, ansi_mode_b);
	// for(int i = 0; i<ansi_args_i; i++)
	// 	printf("ansi arg #%d = %d\n", i, ansi_args[i]);
	if(ansi_mode_a == '[' && ansi_mode_b == 'm')
	{
		for(int i = 0; i<ansi_args_i; i++)
		{
			int A = ansi_args[i];
			if(30 <= A && A <= 37) // fg
				tty_setfg(A - 30);
			else if(40 <= A && A <= 47) // fg
				tty_setbg(A - 40);
			else if(A == 1) // bold
				tty_setfg(tty_getfg() % 8 + 8);
			else if(A == 0)
				tty_reset_color();
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
		tty_putc(c);

	return 0;
}