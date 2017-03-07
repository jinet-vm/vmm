#include <kernel/ints.h>
#include <kernel/ints.h>
#include <kernel/tty.h>
#include <kernel/vga.h>
#include <stdint.h>

extern isr_default_handler();

struct regs
{
	unsigned int gs, fs, es, ds;      /* pushed the segs last */
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
	unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
	unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
};

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();


void ints_install() // default setup
{
	tty_puts("ints_install\n");
	irq_remap();
	idt_set_gate(0, (unsigned)isr0, 0x08, 0x8E);
	idt_set_gate(1, (unsigned)isr1, 0x08, 0x8E);
	idt_set_gate(2, (unsigned)isr2, 0x08, 0x8E);
	idt_set_gate(3, (unsigned)isr3, 0x08, 0x8E);
	idt_set_gate(4, (unsigned)isr4, 0x08, 0x8E);
	idt_set_gate(5, (unsigned)isr5, 0x08, 0x8E);
	idt_set_gate(6, (unsigned)isr6, 0x08, 0x8E);
	idt_set_gate(7, (unsigned)isr7, 0x08, 0x8E);

	idt_set_gate(8, (unsigned)isr8, 0x08, 0x8E);
	idt_set_gate(9, (unsigned)isr9, 0x08, 0x8E);
	idt_set_gate(10, (unsigned)isr10, 0x08, 0x8E);
	idt_set_gate(11, (unsigned)isr11, 0x08, 0x8E);
	idt_set_gate(12, (unsigned)isr12, 0x08, 0x8E);
	idt_set_gate(13, (unsigned)isr13, 0x08, 0x8E);
	idt_set_gate(14, (unsigned)isr14, 0x08, 0x8E);
	idt_set_gate(15, (unsigned)isr15, 0x08, 0x8E);

	idt_set_gate(16, (unsigned)isr16, 0x08, 0x8E);
	idt_set_gate(17, (unsigned)isr17, 0x08, 0x8E);
	idt_set_gate(18, (unsigned)isr18, 0x08, 0x8E);
	idt_set_gate(19, (unsigned)isr19, 0x08, 0x8E);
	idt_set_gate(20, (unsigned)isr20, 0x08, 0x8E);
	idt_set_gate(21, (unsigned)isr21, 0x08, 0x8E);
	idt_set_gate(22, (unsigned)isr22, 0x08, 0x8E);
	idt_set_gate(23, (unsigned)isr23, 0x08, 0x8E);

	idt_set_gate(24, (unsigned)isr24, 0x08, 0x8E);
	idt_set_gate(25, (unsigned)isr25, 0x08, 0x8E);
	idt_set_gate(26, (unsigned)isr26, 0x08, 0x8E);
	idt_set_gate(27, (unsigned)isr27, 0x08, 0x8E);
	idt_set_gate(28, (unsigned)isr28, 0x08, 0x8E);
	idt_set_gate(29, (unsigned)isr29, 0x08, 0x8E);
	idt_set_gate(30, (unsigned)isr30, 0x08, 0x8E);
	idt_set_gate(31, (unsigned)isr31, 0x08, 0x8E);
	
	// irq
	idt_set_gate(32,(unsigned)irq0,0x08,0x8E);
	idt_set_gate(33,(unsigned)irq1,0x08,0x8E);
	idt_set_gate(34,(unsigned)irq2,0x08,0x8E);
	idt_set_gate(35,(unsigned)irq3,0x08,0x8E);
	idt_set_gate(36,(unsigned)irq4,0x08,0x8E);
	idt_set_gate(37,(unsigned)irq5,0x08,0x8E);
	idt_set_gate(38,(unsigned)irq6,0x08,0x8E);
	idt_set_gate(39,(unsigned)irq7,0x08,0x8E);
	idt_set_gate(40,(unsigned)irq8,0x08,0x8E);
	idt_set_gate(41,(unsigned)irq9,0x08,0x8E);
	idt_set_gate(42,(unsigned)irq10,0x08,0x8E);
	idt_set_gate(43,(unsigned)irq11,0x08,0x8E);
	idt_set_gate(44,(unsigned)irq12,0x08,0x8E);
	idt_set_gate(45,(unsigned)irq13,0x08,0x8E);
	idt_set_gate(46,(unsigned)irq14,0x08,0x8E);
	idt_set_gate(47,(unsigned)irq15,0x08,0x8E);
	__asm__ volatile ("sti");
}

static unsigned char *exception_messages[] =
{
	"Division By Zero",
	"Debug",
	"Non Maskable Interrupt",
	"Breakpoint",
	"Into Detected Overflow",
	"Out of Bounds",
	"Invalid Opcode",
	"No Coprocessor",

	"Double Fault",
	"Coprocessor Segment Overrun",
	"Bad TSS",
	"Segment Not Present",
	"Stack Fault",
	"General Protection Fault",
	"Page Fault",
	"Unknown Interrupt",

	"Coprocessor Fault",
	"Alignment Check",
	"Machine Check",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};

void fault_handler(struct regs *r)
{
	if(r->int_no < 32)
	{
		tty_setcolor(vga_color(VC_BLACK, VC_RED));
		tty_puts(exception_messages[r->int_no]);
		tty_puts(" Exception. System Halted!\n");
		for (;;);
	}
}

void irq_remap(void)
{
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x0);
	outb(0xA1, 0x0);
}

void printER(struct regs *r)
{
	tty_puts("test");
}

void *irq_routines[16] =
{
	printER, printER, printER, printER, printER, printER, printER, printER,
	0, 0, 0, 0, 0, 0, 0, 0
};

void irq_handler(struct regs *r)
{
	/* This is a blank function pointer */
	void (*handler)(struct regs *r);

	/* Find out if we have a custom handler to run for this
	*  IRQ, and then finally, run it */
	handler = irq_routines[r->int_no - 32];
	if (handler)
	{
		handler(r);
	}

	/* If the IDT entry that was invoked was greater than 40
	*  (meaning IRQ8 - 15), then we need to send an EOI to
	*  the slave controller */
	if (r->int_no >= 40)
	{
		outb(0xA0, 0x20);
	}

	/* In either case, we need to send an EOI to the master
	*  interrupt controller too */
	outb(0x20, 0x20);
}