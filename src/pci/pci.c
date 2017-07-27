#include <kernel/pci.h>
#include <kernel/io.h>
#include <kernel/module.h>
#include <stdint.h>

#define CONFIG_ADDRESS 0xCF8
#define CONFIG_DATA 0xCFC

MODULE("PCI");

uint32_t pci_read_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
	uint32_t lbus  = (uint32_t)bus;
	uint32_t lslot = (uint32_t)slot;
	uint32_t lfunc = (uint32_t)func;
	uint32_t tmp = 0;
 
	uint32_t address = 0x80000000;
	address |= offset & 0xfc;
	address |= lfunc << 8;
	address |= lslot << 11;
	address |= lbus << 16;
 
 	outl(CONFIG_ADDRESS, address);
	tmp = inl(CONFIG_DATA);
	return tmp;
}

uint8_t pci_read_byte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
	uint32_t dw = pci_read_dword(bus, slot, func, offset & 0xfc);
	return dw >> (offset & 3);
}

uint16_t pci_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
	// it's inefficient but it's decent
	uint8_t high, low;
	low = pci_read_dword(bus, slot, func, offset + 1);
	high = pci_read_dword(bus, slot, func, offset + 1);
	return low & (high << 8);
}

uint16_t getVendorID(uint16_t bus, uint16_t device, uint16_t function)
{
        uint32_t r0 = pci_read_word(bus,device,function,0);
        return r0;
}

uint16_t getDeviceID(uint16_t bus, uint16_t device, uint16_t function)
{
        uint32_t r0 = pci_read_word(bus,device,function,2);
        return r0;
}

uint16_t getClassId(uint16_t bus, uint16_t device, uint16_t function)
{
        uint32_t r0 = pci_read_word(bus,device,function,0xA);
        return (r0 & ~0x00FF) >> 8;
}

uint16_t getSubClassId(uint16_t bus, uint16_t device, uint16_t function)
{
        uint32_t r0 = pci_read_word(bus,device,function,0xA);
        return (r0 & ~0xFF00);
}


// todo
void pci_probe()
{
	for(uint32_t bus = 0; bus < 256; bus++)
    {
        for(uint32_t slot = 0; slot < 32; slot++)
        {
            for(uint32_t function = 0; function < 8; function++)
            {
                    uint16_t vendor = getVendorID(bus, slot, function);
                    if(vendor == 0xffff) continue;
                    //uint16_t device = getDeviceID(bus, slot, function);
                    mprint("%x:%x.%x = %x", bus, slot, function, vendor);
            }
        }
    }
}
