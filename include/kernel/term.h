#ifndef TERM_H
#define TERM_H

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#define MAXTERMS 4

#define TERM_NONE 0
#define TERM_VGA 1
#define TERM_SERIAL_IO 2
#define TERM_SERIAL_MMIO 3

// print term_add result
#define TERM_ADD_RES_PRINT

// todo: magic numbers?
// todo: make vga/vesa separate


struct term_dev
{
	// todo: make mutex great again; see ids from term.c
	char name[16];
	uint8_t type;
	uint64_t addr;
	int (*init)(struct term_dev *t);
	int (*putc)(struct term_dev *t, char c);
};

int term_init();
int term_add(struct term_dev t);
void term_putc(void* s, char c);
void term_print(char *s);
void term_write(char *s, size_t l); // for all added terms
int term_printk(char *fmt, ...);
int term_vprintk(char *fmt, va_list va);

int term_vga_init(struct term_dev* t);
int term_vga_putc(struct term_dev* t, char c);

int term_serial_io_init(struct term_dev* t);
int term_serial_io_putc(struct term_dev* t, char c);

int term_serial_mmio_init(struct term_dev* t);
int term_serial_mmio_putc(struct term_dev* t, char c);

#endif