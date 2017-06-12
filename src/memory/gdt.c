/**
 * @file gdt.c
 * @brief Handles operations with GDT.
 */

#include <kernel/gdt.h>
#include <kernel/consts.h>
#include <kernel/debug.h>
#include <stdint.h>
#include <kernel/printf.h>

/**
 * @brief      Will be sent to GDTR.
 */
// struct GDTP
// {
// 	uint16_t size; ///< Size of GDT
// 	uint64_t off; ///< Offset of GDT
// } __attribute__((packed));

// struct GDTP* gdtp;

#define GDTP_GDT_GAP 8

extern char* setGDTR(struct GDTP* gdtp);

/**
 * @brief      Initialise GDTP
 */
void initGDTR()
{
	mbp;
	gdtp = GDT_VMA_ADDR;
	gdtp->off = GDT_VMA_ADDR+GDTP_GDT_GAP; // right after gdtp
}

// TODO: all the structure stuff and its memory WHATEVER


/**
 * @brief      Returns GDT size.
 *
 * @return     GDT size.
 */
uint16_t GDT_size()
{
	return gdtp->size;
}

/**
 * @brief      Returns GDT offset.
 *
 * @return     GDT offset
 */
uint32_t GDT_offset()
{
	return gdtp->off;
}

/**
 * @brief      Sets GDT entry.
 *
 * @param[in]  num     The number of GDT entry
 * @param[in]  base    The base of GDT entry
 * @param[in]  limit   The limit of GDT entry
 * @param[in]  access  The access flags of GDT entry
 * @param[in]  gran    The granularity flags of GDT entry
 */
// void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran)
// {
// 	gdt_entry* gdte = gdtp->off+num*8;
//     //mbp;
//     gdte->base_low = (base & 0xFFFF);
//     gdte->base_middle = (base >> 16) & 0xFF;
//     gdte->base_high = (base >> 24) & 0xFF;
//     /* Setup the descriptor limits */
//     gdte->limit_low = (limit & 0xFFFF);
//     gdte->granularity = ((limit >> 16) & 0x0F);
//     /* Finally, set up the granularity and access flags */
//     gdte->granularity |= (gran & 0xF0);
//     gdte->access = access;
// }

// void gdt_set_tss(int num, unsigned long base, unsigned long limit, unsigned char gran)
// {
//     gdt_entry* gdte = gdtp->off+num*8;
//     //mbp;
//     gdte->base_low = (base & 0xFFFF);
//     gdte->base_middle = (base >> 16) & 0xFF;
//     gdte->base_high = (base >> 24) & 0xFF;
//     /* Setup the descriptor limits */
//     gdte->limit_low = (limit & 0xFFFF);
//     gdte->granularity = ((limit >> 16) & 0x0F);
//     gdte->granularity |= 
//     /* Finally, set up the granularity and access flags */
//     gdte->granularity |= (gran & 0xF0);
//     gdte->access = access;
// }

void gdt_set_code(int num) // 64 bit, we live in a FLAT
{
	uint64_t* gdte = gdtp->off+num*8;
	uint64_t s;
	s = 0;
	//gdte |= (limit & 0xffff) << 0; // seg.limit [15:00]
	//gdte |= (base & 0xffffff) << 16; // base address 23:00
	s |= 0xalu << 40; // Type: Read/Write, expand-down, accessed
	s |= 1lu << 44; // S -- desc. type: code or data
	s |= 1lu << 47; // P -- present
	//gdte |= (limit & 0xf) << 48;
	s |= 0lu << 52; // AVL: available
	s |= 1lu << 53; // L -- 64-bit code segment
	s |= 0 << 54; // D/B
	s |= 0 << 55; // granularity
	//printf("fuck off code: 0x%x%08x\n", (uint64_t)(s) >> 32, (uint32_t)(s));
	//gdte |= (base && 0xff000000) << 32;
	*gdte = s;
}

void gdt_set_data(int num) // 64 bit, we live in a FLAT
{
	uint64_t* gdte = gdtp->off+num*8;
	uint64_t s;
	s = 0;
	//gdte |= (limit & 0xffff) << 0; // seg.limit [15:00]
	//gdte |= (base & 0xffffff) << 16; // base address 23:00
	s |= 0x2lu << 40; // Type: Read/Write, expand-down, accessed
	s |= 1lu << 44; // S -- desc. type: code or data
	s |= 1lu << 47; // P -- present
	//gdte |= (limit & 0xf) << 48;
	s |= 0lu << 52; // AVL: available
	s |= 1lu << 53; // L -- 64-bit code segment
	s |= 0 << 54; // D/B
	s |= 0 << 55; // granularity
	//printf("fuck off code: 0x%x%08x\n", (uint64_t)(s) >> 32, (uint32_t)(s));
	//gdte |= (base && 0xff000000) << 32;
	*gdte = s;
}

void gdt_set_tss(int num, uint32_t limit, uint64_t base)
{
	uint64_t* gdte = gdtp->off+num*8;
	uint64_t s;
	uint64_t t;
	s = 0;
	s |= (limit & 0xffff) << 0lu; // seg.limit [15:00]
	s |= (base & 0xffffff) << 16lu; // base address 23:00
	s |= 0x9lu << 40lu; // Type: Read/Write, expand-down, accessed
	s |= 0lu << 44lu; // S -- desc. type: code or data
	s |= 1lu << 47lu; // P -- present
	s |= ((limit*1lu) & 0xf0000lu) << 32lu; // limit [19:16]
	s |= 1lu << 52lu; // AVL: available
	//s |= 0lu << 53; // L -- 64-bit code segment
	//s |= 0 << 54; // D/B
	//s |= 0 << 55; // granularity
	s |= (base && 0xff000000lu) << 32lu;
	t = 0;
	t |= (base >> 32);
	*gdte = s;
	gdte++;
	*gdte = t;
}


/**
 * @brief      Flushes GDT into GDTR register.
 *
 * @param[in]  num   The number of GDT entries.
 */
void gdt_flush(int num)
{
	gdtp->size = num*8;
	//printf("fuck off: 0x%x%08x\n", (uint64_t)(gdtp) >> 32, (uint32_t)(gdtp->off));
	asm("xchg %bx, %bx");
	asm("lgdt %0"
		: : "m"(*gdtp));
}