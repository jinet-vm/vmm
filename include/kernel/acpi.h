#ifndef ACPI_H
#define ACPI_H

#include <stdint.h>

struct sdt_header
{
	char sig[4];
	uint32_t length;
	uint8_t rev;
	uint8_t checksum;
	uint8_t oem_id[6];
	uint8_t oemtab_id[8];
	uint32_t oem_rev;
	uint32_t creator_id;
	uint32_t creator_rev;
};

int detect_rsdt();
void print_sdts();
struct sdt_header* find_sdt(char* sig);

#endif