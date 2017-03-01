#include <tss.h>
#include <gdt.h>
#include <stddef.h>
#include <stdint.h>
#include <debug.h>
#include <memory.h>

#define MAIN_TSS 0
#define SS0 0x10
#define ESP0 0

static tss_entry TSS[10];

extern void setTR(uint16_t segment);

void setup_tss(uint32_t num, uint16_t ss0, uint32_t esp0)
{
	// GDT setup
	gdt_set_gate(num,&TSS[num],sizeof(tss_entry), 0xE9, 0x40);
	mbp;
	// Managing TSS
	memset(&TSS[num],0,sizeof(tss_entry));
	TSS[num].ss0 = SS0;
	TSS[num].cs = 0x8;
	TSS[num].es = 0x10;
	TSS[num].fs = 0x10;
	TSS[num].gs = 0x10;
	TSS[num].iomap_base = 0;
}

void enable_tss(uint32_t num)
{
	setTR(SEG(num));
}