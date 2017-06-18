#ifndef GDT_H
#define GDT_H

#include <stdint.h>

#define SEG(i) (i << 3)
#define TSS_SEG 3

#define GDT_SEGS 10

struct GDTP
{
	uint16_t size; ///< Size of GDT
	uint64_t off; ///< Offset of GDT
} __attribute__((packed));

void gdt_flush();
void initGDTR();
struct GDTP* getGDTP();
//void gdt_set_gate(int num, uint64_t base, uint64_t limit, uint8_t access, uint8_t gran);
void gdt_set_code(int num);
void gdt_set_data(int num);

// todo: make GDT saner -- use it in .data

// typedef struct gdt_entry
// {
// 	uint16_t limit_low;
// 	uint16_t base_low;
// 	uint8_t base_middle;
// 	uint8_t access;
// 	uint8_t granularity;
// 	uint8_t base_high;
// } gdt_entry __attribute__((packed));

// typedef struct gdt_tss
// {
// 	uint16_t limit_low;
// 	uint16_t base_low;
// 	uint8_t base_middle;
// 	uint8_t access;
// 	uint8_t granularity;
// 	uint8_t base_high;
// 	uint32_t base_highest;
// 	uint32_t reserved; // [8:12] bits = 0
// } gdt_tss __attribute__((packed));

#endif