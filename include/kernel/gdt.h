#ifndef GDT_H
#define GDT_H

#define SEG(i) (i << 3)
#define TSS_SEG 3

void gdt_flush(int num);
void initGDTR();
void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);

typedef struct gdt_entry
{
	unsigned short limit_low;
	unsigned short base_low;
	unsigned char base_middle;
	unsigned char access;
	unsigned char granularity;
	unsigned char base_high;
} gdt_entry __attribute__((packed));

#endif