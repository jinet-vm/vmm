#ifndef TERM_H
#define TERM_H

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#define MAXTERMS 4
#define MAXTERMBUFS 42

#define TERM_NONE 0
#define TERM_VGA 1
#define TERM_SERIAL_IO 2
#define TERM_SERIAL_MMIO 3
#define TERM_VBE 4

// print term_add result
#define TERM_ADD_RES_PRINT

// todo: magic numbers?
// todo: make vga/vesa separate

void *term_buf;

struct term_dev
{
	// todo: make mutex great again; see ids from term.c
	char name[16];
	uint8_t type;
	uint64_t addr;
	int (*init)(struct term_dev *t);
	int (*putc)(struct term_dev *t, char c);
};

// runs in own thread: the terminal task

// defines hardware
struct termdev
{
	char name[16];
	void* private;
	int (*init)(struct term_dev *t);
	int (*print)(struct term_dev *t, char* str, uint64_t size);
};

int term_task();

// for other tasks

#define TERMBUF_DEFSIZE 64

struct termbuf
{
	enum sync_status {FREE, INWIP, INWAIT, OUTWIP} S;
	void* str;
	size_t len;
};

// defines buffer for a task

int term_init();
int term_add(struct term_dev t);
void term_putc(void* s, char c);
void term_print(char *s);
void term_write(char *s, size_t l); // for all added terms
int term_printk(char *fmt, ...);
int term_vprintk(char *fmt, va_list va);

int term_vga_init(struct term_dev* t);
int term_vga_putc(struct term_dev* t, char c);

int term_vbe_init(struct term_dev* t);
int term_vbe_putc(struct term_dev* t, char c);

int term_serial_io_init(struct term_dev* t);
int term_serial_io_putc(struct term_dev* t, char c);

int term_serial_mmio_init(struct term_dev* t);
int term_serial_mmio_putc(struct term_dev* t, char c);

static struct termbuf* curtb;

#endif