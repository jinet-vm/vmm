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
#include <kernel/gdt.h>
#include <kernel/font.h>
#include <kernel/term.h>
#include <kernel/module.h>
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

void putc(void* none, char c)
{
	tty_putc(c);
}

struct term_dev vga =
{
	.name = "VBE_TERM",
	.type = TERM_VGA,
	.addr = 0x7c00,
	.init = term_vga_init,
	.putc = term_vga_putc
};

struct term_dev com_port =
{
	.name = "COM_PORT",
	.type = TERM_SERIAL_IO,
	.addr = 0x3F8,
	.init = term_serial_io_init,
	.putc = term_serial_io_putc
};

MODULE("MAIN");

void kernel_start()
{
	// VGA
	term_init();
	term_add(vga);
	term_add(com_port);
	for(int i = 1; i<=90; i++)
	 	mprint("%d",i);
	for(;;);
	// IDT
	initGDTR();
	gdt_set_code(1);
	gdt_set_data(2);
	gdt_set_tss(3,104,0xffff800000000000); // host tss - 0x18
	gdt_set_tss(5,104,0xffff800000000100); // vm0 tss - 0x20
	gdt_set_tss(7,104,0xffff800000000200); // vm1 tss - 0x28
	gdt_flush();
	mprint("GDT flushed");
	tr_set(SEG(3));
	mprint("TSS set");
	idt_init();
	isr_install();
	irq_install();
	idt_flush();
	mprint("IDT flushed");
	//volatile int s = 1/0;
	//tty_setcolor(vga_color(VC_LIGHT_GREEN,VC_BLACK));
	mprint("IDT initialized.\n");
	//tty_setcolor(VC_DEFAULT);
	// heap not needed yet
	// // MADT
	detect_rsdt();
	print_sdts();
	uint32_t madtb = detect_madt();
	lapic_setup(); // TODO: apic 32bit bochs error
	// tty_setcolor(vga_color(VC_LIGHT_GREEN,VC_BLACK));
	// mprint("MADT & LAPIC initialized.\n");
	// tty_setcolor(VC_DEFAULT);
	// asm("xchg %bx, %bx");
	//mprint("MADT & LAPIC initialized.\n");
	pic_enable();
	pic_disable();
	ioapic_setup();
	// initGDTR();
	pit_init();
	for(uint8_t i = 0; i<=23; i++)
		ioapic_set_gate(i,34,0,0,0,0,0,0); // just to be on a safe side
	ioapic_set_gate(1,33,0,0,0,0,0,0);
	irq_install_handler(1, keyboard_handler);
	//ints_sti(); //- something wrong with eoi
	//for(;;);
	//for(;;)
	//pit_init();
	//asm("xchg %bx, %bx");
	//ints_sti();
	//for(;;);
	//pit_init();
	// todo: crazy stuff here!
	// VMX
	//mprint("hey!\n");
	//volatile int a = 1/0;
	//asm("int $20");
	//for(;;);
	asm("xchg %bx, %bx");
	mprint("VIRT INIT\n");
	virt_init();
	for(;;);
}
