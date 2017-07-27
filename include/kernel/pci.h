#ifndef PCI_H
#define PCI_H

#include <stdint.h>

struct pci_device
{
	// ?
};

uint8_t pci_read_byte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint16_t pci_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint32_t pci_read_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
void pci_probe();

#endif