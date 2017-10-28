// an interface to serial io

#include <jinet/term.h>
#include <jinet/io.h>

int term_serial_io_init(struct term_dev* t)
{
	unsigned short PORT = t->addr;
	outb(PORT + 1, 0x01);
	outb(PORT + 3, 0x80);
	outb(PORT + 0, 0x03);
	outb(PORT + 1, 0x00);
	outb(PORT + 3, 0x03);
	outb(PORT + 2, 0xC7);
	outb(PORT + 4, 0x0B);
	return 0;
}

static int is_transmit_empty(unsigned short PORT)
{
	return inb(PORT + 5) & 0x20;
}

int term_serial_io_putc(struct term_dev* t, char c)
{
	unsigned short PORT = t->addr;
	while (is_transmit_empty(PORT) == 0);
	outb(PORT, c);
	if(c == '\n')
		term_serial_io_putc(t, '\r');
	return 0;
}