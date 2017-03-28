#include <kernel/idt.h>
#include <kernel/memory.h>
#include <kernel/printf.h>
#include <kernel/debug.h>

volatile static struct idt_header idtr;
int delimit;
volatile static struct idt_entry IDT[IDT_ENTRIES];

void idt_init()
{
	delimit = 0xffff;
	idtr.base = &IDT;
	idtr.limit = sizeof(struct idt_entry)*IDT_ENTRIES;
	//memset(&IDT,0,idtr.limit);
}

void idt_flush()
{
	printf("IDT: 0x%08x%08x\n",(uint64_t)&IDT >> 32, (uint64_t)&IDT & 0xffffffff);
	__asm__("lidt (%0)"
		:
		: "r" ((uint64_t)&idtr)
	);
}

void idt_set_gate(char i, uint64_t off, uint16_t sel, uint8_t flags)
{
	delimit = 0x2a2a;
	IDT[i].offset_low = off & 0xffff;
	IDT[i].offset_middle = (off >> 16) & 0xffff;
	IDT[i].offset_high = off >> 32;
	IDT[i].selector = sel;
	IDT[i].flags = flags << 8;
	IDT[i].always0 = 0;
}