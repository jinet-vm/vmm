// an interface to serial mmio

#include <kernel/term.h>
#include <kernel/io.h>
#include <stdint.h>

// todo: make ioremap-like thing

#define LCR_DLAB_BIT 7
#define RBR_OFF 0x0 // DLAB = 0
#define THR_OFF 0x0 // DLAB = 0
#define DLL_OFF 0x0 // DLAB = 1
#define IER_OFF 0x4 // DLAB = 0
#define DLH_OFF 0x4 // DLAB = 1
#define FCR_OFF 0x8
#define LCR_OFF 0xC
#define MCR_OFF 0x10
#define LSR_OFF 0x14
#define MSR_OFF 0x18

static uint64_t PORT;

static uint8_t serial_in(uint64_t off)
{
	uint8_t *r = PORT+off;
	return *r;
}

static void serial_out(uint64_t off, uint8_t val)
{
	uint64_t *r = PORT+off;
	*r = val;
}

int term_serial_mmio_init(struct term_dev* t)
{
	PORT = t->addr;
	serial_out(LCR_OFF, 0); // DLAB = 0
	serial_out(IER_OFF, 0); // disable interrupts
	serial_out(LCR_OFF, 1 << LCR_DLAB_BIT); // DLAB = 1
	serial_out(DLL_OFF, 1);
	serial_out(DLH_OFF, 0);
	serial_out(LCR_OFF, 0x03);
	serial_out(FCR_OFF, 0xC7);
	serial_out(MCR_OFF, 0x0B);
	return 0;
}

static int is_transmit_empty(void)
{
	return serial_in(LSR_OFF) & 0x20;
}

int term_serial_mmio_putc(struct term_dev* t, char c)
{
	while (is_transmit_empty() == 0);
	serial_out(THR_OFF, c);
	if(c == '\n')
		term_serial_mmio_putc(t, '\r');
	return 0;
}