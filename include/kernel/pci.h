#ifndef PCI_H
#define PCI_H

#include <stdint.h>

// PCI dev: an actual thing stuck in a PC's PCI slot
// PCI device: a mysterious device that might correspond to its physical realization - the PCI DEV
// PCI dev is defined by PCI_DEVN. PCI_DEVN (for us) is basically a uint16_t (it's almost like CONFIG_ADDRESS value) :
// [31-24: reserved][23-16: bus number][15-11: device number][10-8 function][7-0: reserved]

// todo: make it better with bit fields
// everything is better with bit fields
#define pci_devn uint32_t
#define PCI_DEVN(bus, dev, fnc) ((pci_devn)(((fnc & 0x7) << 8) | ((dev & 0x1f) << 11) | ((bus & 0xff) << 16)))
#define PCI_DEVN_BUS(devn) ((pci_devn)((devn >> 16) & 0xff))
#define PCI_DEVN_DEV(devn) ((pci_devn)((devn >> 11) & 0x1f))
#define PCI_DEVN_FNC(devn) ((pci_devn)((devn >> 8) & 0x7))

void pci_probe(void);

uint16_t pci_get_vendor(pci_devn dev);
uint16_t pci_get_device(pci_devn dev);
uint16_t pci_get_class(pci_devn dev);
uint16_t pci_get_subclass(pci_devn dev);
uint32_t pci_get_bar0(pci_devn dev);
uint16_t pci_get_command(pci_devn dev);
void pci_set_command(pci_devn dev, uint16_t value);


struct pci_device_id
{
	uint32_t vendor;
	uint32_t device;
};

pci_devn pci_find_device(struct pci_device_id d);

// PCI COMMAND register options
#define PCI_CMD_IO 0
#define PCI_CMD_MM 1
#define PCI_CMD_BUS_M 2
#define PCI_CMD_MWIE 4

#endif