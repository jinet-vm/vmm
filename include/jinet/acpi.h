#ifndef ACPI_H
#define ACPI_H
#include <stdint.h>

#define ACPI_CHECK_SUM

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
} __attribute__((packed));

//int acpi_detect_rsdt();
int acpi_probe();
int acpi_add_driver();
//struct sdt_header* acpi_find_sdt(char* sig);

#endif