/**
 * @file memtab.c
 * @brief Interface on top of GDT and TSS.
 */

#include <kernel/memtab.h>
#include <kernel/memory.h>
#include <kernel/gdt.h>
#include <kernel/tss.h>
#include <kernel/debug.h>

/**
 * @brief      Handles memory setup - GDT and TSS.
 */
void mem_setup()
{
	initGDTR();
	//mbp;
	memset(GDT_offset(),0,sizeof(gdt_entry)*8);
	mbp;
	gdt_set_gate(0,0,0,0,0); // NULL
	//mbp;
	gdt_set_gate(1,0,0xFFFFFFFF,0x9A,0xCF); // code segment
	gdt_set_gate(2,0,0xFFFFFFFF,0x92,0xCF); // data segment
	gdt_set_gate(3,0,0xFFFFFFFF,0xFA,0xCF); // user code segment
	gdt_set_gate(4,0,0xFFFFFFFF,0xF2,0xCF); // user code segment
	setup_tss(5,0x10,0);
	setup_tss(6,0x10,0);
	//mbp;
	gdt_flush(7);
}