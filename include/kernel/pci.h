#ifndef PCI_H
#define PCI_H

#include <stdint.h>

struct pci_device
{
	uint16_t bus;
	uint16_t dev;
	uint16_t fnc;
};

void pci_probe(void);

uint16_t pci_get_vendor(struct pci_device dev);
uint16_t pci_get_device(struct pci_device dev);
uint16_t pci_get_class(struct pci_device dev);
uint16_t pci_get_subclass(struct pci_device dev);

uint16_t pci_get_command(struct pci_device dev);
void pci_set_command(struct pci_device dev, uint16_t value);

// PCI COMMAND register options
#define PCI_CMD_IO 0
#define PCI_CMD_MM 1
#define PCI_CMD_BUS_M 2

#endif