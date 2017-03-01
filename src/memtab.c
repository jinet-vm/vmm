#include <memtab.h>
#include <memory.h>
#include <gdt.h>
#include <tss.h>

void mem_setup()
{
	initGDTR();
	memset(GDT_offset(),0,sizeof(gdt_entry)*8);
	gdt_set_gate(0,0,0,0,0); // NULL
	gdt_set_gate(1,0,0xFFFFFFFF,0x9A,0xCF); // code segment
	gdt_set_gate(2,0,0xFFFFFFFF,0x92,0xCF); // data segment
	gdt_set_gate(3,0,0xFFFFFFFF,0xFA,0xCF); // user code segment
	gdt_set_gate(4,0,0xFFFFFFFF,0xF2,0xCF); // user code segment
	setup_tss(5,0x10,0);
	setup_tss(6,0x10,0);
	gdt_flush(7);
}