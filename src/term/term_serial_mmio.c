// an interface to serial mmio

#include <kernel/term.h>
#include <kernel/module.h>
#include <kernel/io.h>
#include <kernel/pci.h>
#include <stdint.h>

MODULE("TERM_MMIO");

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
#define SRR_OFF 0x88

static uint8_t serial_in(uint64_t PORT, uint64_t off)
{
	uint8_t *r = 0xef253000+off;
	return *r;
}

static void serial_out(uint64_t PORT, uint64_t off, uint8_t val)
{
	uint64_t *r = 0xef253000+off;
	*r = val;
}

int term_serial_mmio_init(struct term_dev* t)
{
	pci_devn d = pci_find_device((struct pci_device_id){.vendor = 0x8086, 0xA166});
	if(d == (pci_devn)(~0))
	{
		mprint("PCI device not found");
		return -1;
	}
	pci_set_command(d, (1 << PCI_CMD_MM) | (1 << PCI_CMD_BUS_M));
	pci_set_cache_line_size(d, 0x10);
	pci_dump(d);
	t->addr = pci_get_bar0(d) & (~0xF);
	mprint("MMIO addr: %8x", t->addr);
	uint64_t PORT = 0xef253000; // todo: pci!
	serial_in(PORT,0xf8);
	serial_in(PORT,0xf4);
	serial_out(PORT, 0x10, 0x1); // defines!
	serial_out(PORT, 0x4, 0x5);
	serial_out(PORT, 0xc, 0x93);
	serial_out(PORT, 0x0, 0x80);
	serial_out(PORT, 0x4, 0x4);
	serial_out(PORT, 0xc, 0x13);
	serial_out(PORT, 0x8, 0x1); // resets XMIT and RCVR...
	serial_out(PORT, 0x8, 0x87);
	serial_out(PORT, 0x10, 0xB);
	serial_in(PORT, 0x4);
	serial_out(PORT, 0x4, 0x0);
	return 0;
}

static int is_transmit_empty(uint64_t PORT)
{
	return serial_in(0xef253000, LSR_OFF) & 0x20;
}

int term_serial_mmio_putc(struct term_dev* t, char c)
{
	uint64_t PORT = 0xef253000;
	while (is_transmit_empty(PORT) == 0);
	serial_out(PORT, THR_OFF, c);
	if(c == '\n')
		term_serial_mmio_putc(t, '\r');
	return 0;
}