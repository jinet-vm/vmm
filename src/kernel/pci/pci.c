// both convPCI and universal PCI procedures

#include <jinet/pci.h>
#include <jinet/io.h>
#include <jinet/module.h>
#include <jinet/printf.h>
#include <stdint.h>

#define CONFIG_ADDRESS 0xCF8
#define CONFIG_DATA 0xCFC

MODULE("PCI");

uint16_t (*pci_read_word)(pci_devn dev, uint16_t offset);
void (*pci_write_word)(pci_devn dev, uint16_t offset, uint16_t value);
void (*pci_probe)();

static pci_devn devns[32];
static int devs_i = 0;

void pci_add_devn(pci_devn d)
{
	devns[devs_i++] = d;
}

static uint16_t cpci_read_word(pci_devn dev, uint16_t offset)
{
	if(offset > 64) return 0;
	uint64_t lbus = (uint64_t)PCI_DEVN_BUS(dev);
	uint64_t ldev = (uint64_t)PCI_DEVN_DEV(dev);
	uint64_t lfnc = (uint64_t)PCI_DEVN_FNC(dev);
	uint16_t tmp = 0;
	uint64_t address = 0x80000000;
	address |= offset & 0xfc;
	address |= lfnc << 8;
	address |= ldev << 11;
	address |= lbus << 16;
	outl (CONFIG_ADDRESS, address);
	tmp = (uint16_t)((inl(CONFIG_DATA) >> ((offset & 2) * 8)) & 0xffff);
	return tmp;
}

static void cpci_write_word(pci_devn dev, uint16_t offset, uint16_t value)
{
	if(offset > 64) return;
	uint64_t lbus = (uint64_t)PCI_DEVN_BUS(dev);
	uint64_t ldev = (uint64_t)PCI_DEVN_DEV(dev);
	uint64_t lfnc = (uint64_t)PCI_DEVN_FNC(dev);
	uint64_t address = 0x80000000;
	uint32_t tmp = 0;
	address |= offset & 0xfc;
	address |= lfnc << 8;
	address |= ldev << 11;
	address |= lbus << 16;
	outl(CONFIG_ADDRESS, address);
	tmp = inl(CONFIG_DATA);
	tmp &= ~(0xffff << ((offset & 2) * 8));
	tmp |= value << ((offset & 2) * 8);
	outl(CONFIG_DATA, tmp);
	return (tmp);
}

static void cpci_probe(void);

void cpci_init()
{
	pci_read_word = cpci_read_word;
	pci_write_word = cpci_write_word;
	pci_probe = cpci_probe;
}

uint16_t pci_get_vendor(pci_devn dev)
{
	uint16_t r0 = pci_read_word(dev, 0);
	return r0;
}

uint16_t pci_get_device(pci_devn dev)
{
	uint16_t r0 = pci_read_word(dev, 2);
	return r0;
}

uint16_t pci_get_class(pci_devn dev)
{
	uint16_t r0 = pci_read_word(dev, 0xA);
	return (r0 & 0xFF00) >> 8;
}

uint16_t pci_get_subclass(pci_devn dev)
{
	uint16_t r0 = pci_read_word(dev, 0xA);
	return (r0 & 0x00FF);
}

uint32_t pci_get_bar0(pci_devn dev)
{
	uint32_t r0 = pci_read_word(dev, 0x10);
	r0 |= pci_read_word(dev, 0x12) << 16;
	return r0;
}

uint16_t pci_get_command(pci_devn dev)
{
	uint16_t r0 = pci_read_word(dev, 0x4);
	return r0;
}

uint16_t pci_get_status(pci_devn dev)
{
	uint16_t r0 = pci_read_word(dev, 0x6);
	return r0;
}

void pci_set_command(pci_devn dev, uint16_t value)
{
	pci_write_word(dev, 0x4, value);
}

static void cpci_probe(void)
{
	pci_devns_clean();
	for(uint32_t bus = 0; bus < 256; bus++)
		for(uint32_t dev = 0; dev < 32; dev++)
			for(uint32_t fnc = 0; fnc < 8; fnc++)
			{
				pci_devn d = PCI_DEVN(bus,dev,fnc);
				uint16_t vendor = pci_get_vendor(d);
				if(vendor == 0xffff) continue;
				pci_add_devn(d);
			}
	pci_list();
	mprint("conventional PCI initialized");
}

void pci_devns_clean()
{
	devs_i = 0;
}

void pci_list()
{
	for(int i = 0; i<devs_i; i++)
	{
		pci_devn d = devns[i];
		mprint("%02x:%02x.%x => vendor %4xh, device %4xh", PCI_DEVN_BUS(d), PCI_DEVN_DEV(d), PCI_DEVN_FNC(d), pci_get_vendor(d), pci_get_device(d));
	}
}

pci_devn pci_find_device(struct pci_device_id d)
{
	for(int i = 0; i<devs_i; i++)
	{
		pci_devn D = devns[i];
		if(pci_get_vendor(D) == d.vendor && pci_get_device(D) == d.device)
			return D;
		//pci_dump(D);
	}
	return (pci_devn)(~0);
}

void pci_set_cache_line_size(pci_devn dev, uint8_t value)
{
	uint16_t r0 = pci_read_word(dev, 0x0C);
	r0 &= 0xff00;
	r0 |= value;
	pci_write_word(dev,0x0C,r0);
}

void pci_dump(pci_devn d)
{
	mprint("pci device dump:");
	for(int i = 0; i < 0x10; i++)
	{
		printf("%02x: ", i*0x10);
		for(int j = 0; j<8; j++)
		{
			uint16_t t = pci_read_word(d,0x10*i+2*j);
			printf("%02x ", t & 0xff);
			printf("%02x ", (t >> 8) & 0xff);
		}
		printf("\n");
	}
}