#include <gdt.h>
#include <stdint.h>
extern char* getGDTR();
extern char* setGDTR();

struct GDTP
{
	uint16_t size;
	uint32_t off;
} __attribute__((packed));

static struct GDTP* gdtp;


void initGDTR()
{
	gdtp = getGDTR();
}

// TODO: all the structure stuff and its memory WHATEVER

uint16_t GDT_size()
{
	return gdtp->size;
}

uint32_t GDT_offset()
{
	return gdtp->off;
}

void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran)
{
	gdt_entry* gdte = gdtp->off+num*8;
    gdte->base_low = (base & 0xFFFF);
    gdte->base_middle = (base >> 16) & 0xFF;
    gdte->base_high = (base >> 24) & 0xFF;
    /* Setup the descriptor limits */
    gdte->limit_low = (limit & 0xFFFF);
    gdte->granularity = ((limit >> 16) & 0x0F);
    /* Finally, set up the granularity and access flags */
    gdte->granularity |= (gran & 0xF0);
    gdte->access = access;
}

void gdt_flush(int num)
{
	gdtp->size = num*8;
	setGDTR();
}