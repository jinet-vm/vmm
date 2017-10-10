#ifndef DBGP_H
#define DBGP_H

#include <stdint.h>

#define GRAS_MMIO	0
#define GRAS_IO		1
#define GRAS_PCI	2

struct gras // Generic Register Address Structure
{
	uint8_t addr_space;
	uint8_t reg_bit_width;
	uint8_t reg_bit_offset;
	uint8_t _r0;
	uint64_t addr;
} __attribute__ ((packed));

int dbgp_probe(void* table);
uint64_t dbgp_base_addr();

#endif