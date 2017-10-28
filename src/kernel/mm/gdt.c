/**
 * @file gdt.c
 * @brief Handles operations with GDT.
 */

#include <jinet/gdt.h>
#include <jinet/consts.h>
#include <jinet/debug.h>
#include <stdint.h>
#include <jinet/printf.h>

static uint64_t gdt[GDT_SEGS];
static struct GDTP gdtp;

// struct GDTP* gdtp;

#define GDTP_GDT_GAP 8

extern char* setGDTR(struct GDTP* gdtp);

/**
 * @brief      Initialise GDTP
 */
void initGDTR()
{
	mbp;
	gdtp.off = gdt; // right after gdtp
	gdtp.size = GDT_SEGS*8;
}

struct GDTP* getGDTP()
{
	return &gdtp;
}

// TODO: all the structure stuff and its memory WHATEVER


/**
 * @brief      Returns GDT size.
 *
 * @return     GDT size.
 */
uint16_t GDT_size()
{
	return gdtp.size;
}

/**
 * @brief      Returns GDT offset.
 *
 * @return     GDT offset
 */
uint32_t GDT_offset()
{
	return gdtp.off;
}

void gdt_set_code(int num) // 64 bit, we live in a FLAT
{
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
	gdt[num] = s;
}

void gdt_set_data(int num) // 64 bit, we live in a FLAT
{
	//uint64_t* gdte = gdtp->off+num*8;
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
	gdt[num] = s;
}

void gdt_set_tss(int num, uint32_t limit, uint64_t base)
{
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
	gdt[num] = s;
	gdt[num+1] = t;
}


/**
 * @brief      Flushes GDT into GDTR register.
 *
 * @param[in]  num   The number of GDT entries.
 */
void gdt_flush()
{
	//printf("fuck off: 0x%x%08x\n", (uint64_t)(gdtp) >> 32, (uint32_t)(gdtp->off));
	asm("lgdt %0"
		: : "m"(gdtp));
}