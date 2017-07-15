// just a demo!

#include <kernel/tty.h>
#include <kernel/io.h>
#include <kernel/irq.h>
#include <kernel/printf.h>

unsigned char kbdus[128] =
{
	0,
	27,
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8', /* 9 */
	'9',
	'0',
	'-',
	'=',
	'\b',/* Backspace */
	'\t', /* Tab */
	'q',
	'w',
	'e',
	'r', /* 19 */
	't',
	'y',
	'u',
	'i',
	'o',
	'p',
	'[',
	']',
	'\n', /* Enter key */
	0, /* 29   - Control */
	'a',
	's',
	'd',
	'f',
	'g',
	'h',
	'j',
	'k',
	'l',
	';', /* 39 */
	'\'',
	'`',
	0xF,/* Left shift */
	'\\',
	'z',
	'x',
	'c',
	'v',
	'b',
	'n', /* 49 */
	'm',
	',',
	'.',
	'/',
	0xF, /* Right shift */
	'*',
	0,	/* Alt */
	' ',	/* Space bar */
	0,	/* Caps lock */
	0,	/* 59 - F1 key ... > */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	/* < ... F10 */
	0,
	/* 69 - Num lock*/
	0,
	/* Scroll Lock */
	0,
	/* Home key */
	0,
	/* Up Arrow */
	0,
	/* Page Up */
	'-',

	0,
	/* Left Arrow */
	0,

	0,
	/* Right Arrow */
	'+',

	0,
	/* 79 - End key*/
	0,
	/* Down Arrow */
	0,
	/* Page Down */
	0,
	/* Insert Key */
	0,
	/* Delete Key */
	0,
	0,
	0,
	0,
	/* F11 Key */
	0,
	/* F12 Key */
	0,
	/* All other keys are undefined */
};

void keyboard_handler(struct regs *r)
{
	asm("xchg %bx, %bx");
	const char shift = 0;
	static char flags = 0;
	unsigned char scancode = inb(0x60);
	if(scancode & 0x80) // released
	{
		if(scancode == 0xaa)
		{
			flags ^= 1 << shift;
		}
	}
	else // pressed
	{
		if(scancode == 0x2a)
		{
			flags ^= 1 << shift;
		}
		char inp = kbdus[scancode];
		if(('a' <= inp && inp <= 'z') ||
			('0' <= inp && inp <= '9') ||
			inp == '\n' || inp == ' ')
			term_putc(inp+(flags & (1 << shift) ? 'A'-'a' : 0));
	}
}