// MCFG and enhanced PCI

#include <jinet/mcfg.h>
#include <jinet/acpi.h>
#include <jinet/pci.h>
#include <jinet/module.h>
#include <stdint.h>

MODULE("ACPI_PCIE");

struct pci_conf_space
{
	uint64_t base_address;
	uint16_t seg_grp_num;
	uint8_t start_bus;
	uint8_t end_bus;
	uint32_t reserved;
};

struct mcfg
{
	struct sdt_header h;
	uint64_t reserved;
	struct pci_conf_space pcd[];
} __attribute__ ((packed));

struct mcfg* MCFG;

static int sgn = 0;

int mcfg_probe(void* table)
{
	MCFG = table;
	sgn = (MCFG->h.length - (sizeof(struct sdt_header) + 8)) / sizeof(struct pci_conf_space);
	pcie_init();
}

uint64_t mcfg_seg_group_count(void)
{
	if(!MCFG || !sgn)
		return MCFG_INVALID;
	return sgn;
}

uint64_t mcfg_seg_group_bar(uint16_t sg)
{
	return MCFG->pcd[sg].base_address;
}

uint64_t mcfg_seg_group_start_bus(uint16_t sg)
{
	return MCFG->pcd[sg].start_bus;
}

uint64_t mcfg_seg_group_end_bus(uint16_t sg)
{
	return MCFG->pcd[sg].end_bus;
}

static uint16_t pcie_read_word(pci_devn dev, uint16_t offset);
static void pcie_write_word(pci_devn dev, uint16_t offset, uint32_t value);
static void pcie_probe();

void pcie_init()
{
	if(!MCFG)
	{
		mprint("no MCFG: can't initialized PCIe");
		return;
	}
	pci_read_word = pcie_read_word;
	pci_write_word = pcie_write_word;
	pci_probe = pcie_probe;
}

// todo: ioremap-style pages!
static uint16_t pcie_read_word(pci_devn dev, uint16_t offset)
{
	uint64_t lsgp = PCI_DEVN_SGP(dev);
	uint64_t lbus = (uint64_t)PCI_DEVN_BUS(dev);
	uint64_t ldev = (uint64_t)PCI_DEVN_DEV(dev);
	uint64_t lfnc = (uint64_t)PCI_DEVN_FNC(dev);
	uint16_t *r =  mcfg_seg_group_bar(lsgp) + ((lbus - mcfg_seg_group_start_bus(dev)) << 20 | ldev << 15 | lfnc << 12) + offset;
	return *r;
}

static void pcie_write_word(pci_devn dev, uint16_t offset, uint32_t value)
{
	uint64_t lsgp = PCI_DEVN_SGP(dev);
	uint64_t lbus = (uint64_t)PCI_DEVN_BUS(dev);
	uint64_t ldev = (uint64_t)PCI_DEVN_DEV(dev);
	uint64_t lfnc = (uint64_t)PCI_DEVN_FNC(dev);
	uint16_t *r =  mcfg_seg_group_bar(lsgp) + ((lbus - mcfg_seg_group_start_bus(dev)) << 20 | ldev << 15 | lfnc << 12) + offset;
	*r = value;
}

static void pcie_probe()
{
	pci_devns_clean();
	mprint("%x %x", mcfg_seg_group_start_bus(0), mcfg_seg_group_end_bus(0));
	//return;
	uint16_t sgpn = mcfg_seg_group_count(), bend;
	for(uint16_t sgp = 0; sgp<sgpn; sgp++)
		for(uint32_t bus = mcfg_seg_group_start_bus(sgp), bend = mcfg_seg_group_end_bus(sgp); bus < 3; bus++) // todo: bug fix; what's wrong?!
			for(uint32_t dev = 0; dev < 32; dev++)
				for(uint32_t fnc = 0; fnc < 8; fnc++)
				{
					pci_devn d = PCI_DEVN(bus,dev,fnc);
					uint16_t vendor = pci_get_vendor(d);
					if(vendor == 0xffff) continue;
					pci_add_devn(d);
				}
	pci_list();
	mprint("enhanced PCI initialized");
}