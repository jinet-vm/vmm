#include <kernel/io.h>
#include <kernel/ints.h>
#include <stdint.h>
#include <kernel/debug.h>
#include <kernel/apic.h>

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

#define PIC1_CMD		0x20
#define PIC1_DATA		0x21
#define PIC2_CMD		0xA0
#define PIC2_DATA		0xA1
#define PIC_READ_IRR	0x0a
#define PIC_READ_ISR	0x0b

#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */
 
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

// io_wait is not needed - we're in a goddamn 21st centrury

// todo: get outta here!1
void pic_enable()
{
	outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
	outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
	outb(PIC1_DATA, 0x20);
	outb(PIC2_DATA, 0x28);
	outb(PIC1_DATA, 0x04);
	outb(PIC2_DATA, 0x02);
	outb(PIC1_DATA, 0x01);
	outb(PIC2_DATA, 0x01);
	outb(PIC1_DATA, 0x0);
	outb(PIC2_DATA, 0x0);
	// todo: understand what is this
}

void pic_disable() // info: see http://ethv.net/workshops/osdev/notes/notes-3s
{
	// info: see http://wiki.osdev.org/8259_PIC
	// set ICW1
	// outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
	// outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);

	// // set ICW2
	// outb(PIC2_DATA, 0xe0);
	// outb(PIC1_DATA, 0xe8);

	// // set ICW3
	// outb(PIC1_DATA, 4);
	// outb(PIC2_DATA, 2);

	// // set ICW4
	// outb(PIC1_DATA, 1);
	// outb(PIC2_DATA, 1);

	// set OCW1
	outb(PIC1_DATA, 0xFF);
	outb(PIC2_DATA, 0xFF);
	// todo: what the heck is this?
}

void irq_install()
{
	//pic_enable();
	idt_set_gate(32,(uint64_t)irq0,0x08,0x8E);
	idt_set_gate(33,(uint64_t)irq1,0x08,0x8E);
	idt_set_gate(34,(uint64_t)irq2,0x08,0x8E);
	idt_set_gate(35,(uint64_t)irq3,0x08,0x8E);
	idt_set_gate(36,(uint64_t)irq4,0x08,0x8E);
	idt_set_gate(37,(uint64_t)irq5,0x08,0x8E);
	idt_set_gate(38,(uint64_t)irq6,0x08,0x8E);
	idt_set_gate(39,(uint64_t)irq7,0x08,0x8E);
	idt_set_gate(40,(uint64_t)irq8,0x08,0x8E);
	idt_set_gate(41,(uint64_t)irq9,0x08,0x8E);
	idt_set_gate(42,(uint64_t)irq10,0x08,0x8E);
	idt_set_gate(43,(uint64_t)irq11,0x08,0x8E);
	idt_set_gate(44,(uint64_t)irq12,0x08,0x8E);
	idt_set_gate(45,(uint64_t)irq13,0x08,0x8E);
	idt_set_gate(46,(uint64_t)irq14,0x08,0x8E);
	idt_set_gate(47,(uint64_t)irq15,0x08,0x8E);
}

void *irq_routines[16] =
{
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0
};

void irq_install_handler(int irq, void (*handler)(struct regs*))
{
	irq_routines[irq] = handler;
}

void irq_handler(struct regs *r)
{
	printf("irq %d", r->int_no);
	void (*handler)(struct regs*);

	if (r->int_no >= INTS_MAX_IRQ)
	{
		lapic_eoi_send();
		return;
	}

	handler = irq_routines[r->int_no - INTS_MAX_ISR];
	if (handler)
	{
		handler(r);
	}

	// EOI
	lapic_eoi_send();
}