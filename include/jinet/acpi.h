/** @file
 *	@brief      ACPI handler
 */

#ifndef ACPI_H
#define ACPI_H
#include <stdint.h>

#define ACPI_CHECK_SUM


/**
 * @brief      Common header for all ACPI tables.
 */
struct sdt_header
{
	/// @brief      Signature
	char sig[4];
	/// @brief      Length (in bytes) of a table
	uint32_t length;
	/// @brief      Revision
	uint8_t rev;
	/// @brief      Checksum
	uint8_t checksum;
	/// @brief      OEM id
	uint8_t oem_id[6];
	/// @brief      OEM table id
	uint8_t oemtab_id[8];
	/// @brief      OEM revision
	uint32_t oem_rev;
	/// @brief      Creator id
	uint32_t creator_id;
	/// @brief      Creator revision
	uint32_t creator_rev;
} __attribute__((packed));

/// @brief      Parse through ACPI tables
int acpi_probe();
/// @brief Add handler for an ACPI table that will be used in #acpi_probe.
int acpi_add_driver();

#endif