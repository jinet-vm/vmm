/**
 * @file gdt.c
 * @brief Handles operations with GDT.
 */

#include <kernel/gdt.h>
#include <kernel/consts.h>
#include <kernel/debug.h>
#include <stdint.h>

/**
 * @brief      Will be sent to GDTR.
 */
struct GDTP
{
	uint16_t size; ///< Size of GDT
	uint32_t off; ///< Offset of GDT
} __attribute__((packed));

struct GDTP* gdtp;

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
void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran)
{
	gdt_entry* gdte = gdtp->off+num*8;
    //mbp;
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

/**
 * @brief      Flushes GDT into GDTR register.
 *
 * @param[in]  num   The number of GDT entries.
 */
void gdt_flush(int num)
{
	gdtp->size = num*8;
	setGDTR(gdtp);
}