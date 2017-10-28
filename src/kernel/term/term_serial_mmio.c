// an interface to serial mmio

#include <jinet/term.h>
#include <jinet/module.h>
#include <jinet/io.h>
#include <jinet/pci.h>
#include <stdint.h>

MODULE("TERM_MMIO");

// todo: make ioremap-like thing

#define UART_THR		0x00
#define UART_DLL		0x00
#define UART_DLM		0x01
#define UART_LCR        0x03
#define UART_MCR        0x04
#define UART_LSR        0x05
#define UART_USR		0x1F
uint8_t *pBase;

int term_serial_mmio_init(struct term_dev* t)
{
	mprint("addr mmio: %llx", t->addr);
	pBase = (uint32_t*)t->addr;
	// set baud rate = 115200
	pBase[UART_LCR] = 0x80; // set DLAB
	pBase[UART_DLM] = 0;
	pBase[UART_DLL] = 1;
 	// reset DLAB and set 8-n-1 mode (8 bits, no-Parity, 1 stop bit)
 	pBase[UART_LCR] = 3;
	// switch off H/W Flow control and set RTS
	pBase[UART_MCR] = 2;
	mprint("usr: %x", pBase[UART_USR]);
	mprint("lsr: %x", pBase[UART_LSR]);
	return 0;
}

// TODO: more effective string-way

int term_serial_mmio_putc(struct term_dev* t, char c)
{
	while ((pBase[UART_LSR] & 0x20) == 0);
	pBase[UART_THR] = c;
	return 0;
}

// reference:
// https://download.01.org/future-platform-configuration-hub/skylake/register-definitions/332219-002.pdf
// https://linux-sunxi.org/images/d/d2/Dw_apb_uart_db.pdf