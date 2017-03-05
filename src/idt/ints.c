#include <kernel/ints.h>
#include <kernel/ints.h>
#include <stdint.h>

extern isr_default_handler();

void ints_install() // default setup
{
	for(int i = 1; i<256; i++)
		idt_set_gate(i,(uint32_t)isr_default_handler, 0x8,0x8E);
	//idt_flush();
}