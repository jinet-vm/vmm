#include <kernel/io.h>
#include <kernel/ints.h>
#include <stdint.h>
#include <kernel/debug.h>

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

void pic_enable()
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

void pic_disable()
{
	outb(0xA1, 0xFF);
	outb(0x21, 0xFF);
}

void irq_install()
{
	pic_enable();
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
	mbp;
	//printf("irq %d", r->int_no);
	void (*handler)(struct regs*);

	if (r->int_no >= INTS_MAX_IRQ)
	{
		outb(0xA0, 0x20); // wrong int_no => EOI
	}

	handler = irq_routines[r->int_no - INTS_MAX_ISR];
	if (handler)
	{
		handler(r);
	}

	// EOI
	outb(0x20, 0x20);
	mbp;
}