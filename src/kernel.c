/**
 * @file kernel.c
 * @brief Main kernel file.
 */

#include <stdint.h>
//#include <stdlib.h>
#include <stddef.h>
#include <kernel/debug.h>
#include <kernel/ints.h>
#include <kernel/idt.h>
#include <kernel/printf.h>
#include <kernel/keyboard.h>
#include <kernel/acpi.h>
#include <kernel/madt.h>
#include <kernel/heap.h>
#include <kernel/ipi.h>
#include <kernel/consts.h>
#include <kernel/ioapic.h>
#include <kernel/vmx.h>

#define p_entry(addr, f) (addr << 12) | f

#define title_lines 6

char* title[title_lines] = 
{
	"                         \xDC\xDC\xDC\xDC\xDC \xDC    \xDC   \xDC\xDB\xDB\xDB\xDC     \xDC\xDC\xDC\xDC\xDF v0.0.1 (WIP)\n",
	"                       \xDC\xDF  \xDB   \xDB\xDB     \xDB  \xDB\xDF   \xDF \xDF\xDF\xDF \xDB    \n",
	"                           \xDB   \xDB\xDB \xDB\xDB   \xDB \xDB\xDB\xDC\xDC       \xDB    \n",
	"                        \xDC \xDB    \xDE\xDB \xDB \xDB  \xDB \xDB\xDC   \xDC\xDF   \xDB     \n",
	"                         \xDF      \xDE \xDB  \xDB \xDB \xDF\xDB\xDB\xDB\xDF    \xDF      \n",
	"                                  \xDB   \xDB\xDB\n\n"
};

extern ints_sti(void);

void kernel_start()
{
	// VGA
	vga_init();
	tty_init();
	tty_setcolor(vga_color(VC_LIGHT_BLUE,VC_BLACK));
	for(int i = 0; i<title_lines; i++)
		tty_puts(title[i]);
	tty_setcolor(VC_DEFAULT);
	tty_setcolor(vga_color(VC_LIGHT_GREEN,VC_BLACK));
	printf("VGA terminal initialized.\n");
	// IDT
	idt_init();
	isr_install();
	irq_install();
	idt_flush();
	tty_setcolor(vga_color(VC_LIGHT_GREEN,VC_BLACK));
	printf("IDT initialized.\n");
	tty_setcolor(VC_DEFAULT);
	// heap not needed yet
	// MADT
	detect_rsdt();
	print_sdts();
	uint32_t madtb = detect_madt();
	lapic_setup(); // TODO: apic 32bit bochs error
	tty_setcolor(vga_color(VC_LIGHT_GREEN,VC_BLACK));
	printf("MADT & LAPIC initialized.\n");
	tty_setcolor(VC_DEFAULT);
	//pic_disable();
	//ioapic_setup();
	//asm("sti");
	//pit_init();
	// todo: crazy stuff here!
	// VMX
	vmx_init();
	for(;;);
}