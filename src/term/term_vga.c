// an interface to vga functions

#include <kernel/term.h>

// todo: it's horrible, make vga buffer great again (not the hardcoded 0x7c00)

int term_vga_init(struct term_dev* t)
{
	vga_init();
	tty_init();
	return 0;
}

int term_vga_putc(struct term_dev* t, char c)
{
	tty_putc(c);
	return 0;
}