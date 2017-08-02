#include <kernel/acpi.h>
#include <kernel/module.h>
#include <stdint.h>

#define EBDA_P_OFF 0x40E

MODULE("ACPI");

struct rsdp_1_0
{
	char sig[8];
	uint8_t checksum;
	char oem[6];
	uint8_t rev;
	uint32_t rsdt_off;
} __attribute__ ((packed));

struct rsdp_2_0
{
	struct rsdp_1_0 fp;
	uint32_t length;
	uint64_t xsdt_off;
	uint8_t reserved;
} __attribute__ ((packed));

struct rsdt
{
	struct sdt_header h;
	uint32_t sdt_p[];
} __attribute__ ((packed));

static struct rsdp_2_0* RSDP = 0;
static struct rsdt* RSDT = 0;

static int acpi_detect_rsdt();
static void acpi_sdts_probes();

struct acpi_driver
{
	char* sig;
	int (*probe)(void* table);
};

static char probed = 0; // make it sane

static struct acpi_driver ads[32];
int ads_i = 0;

int acpi_add_driver(char* sig, int (*probe)(void* table))
{
	if(probed)
		mprint("Driver addition should be done before the probe");
	ads[ads_i++] = (struct acpi_driver){.sig = sig, .probe = probe};
	return 0;
}

int acpi_probe()
{
	if(acpi_detect_rsdt()) return -1;
	acpi_sdts_probes();
	return 0;
}

static void acpi_sdts_probes()
{
	uint32_t ent = ((RSDT->h.length)-(uint32_t)sizeof(RSDT->h))/sizeof(RSDT->sdt_p[0]);
	for(int i = 0; i<ent; i++)
	{
		struct sdt_header* sh = RSDT->sdt_p[i];
		mprint("Found %c%c%c%c", sh->sig[0], sh->sig[1], sh->sig[2], sh->sig[3]);
		for(int j = 0; j<ads_i; j++)
			if(	ads[j].sig[0] == sh->sig[0] &&
				ads[j].sig[1] == sh->sig[1] &&
				ads[j].sig[2] == sh->sig[2] &&
				ads[j].sig[3] == sh->sig[3]) // found!
					ads[j].probe(sh);
	}
	probed = 1;
	return 0;
}

static int acpi_detect_rsdt()
{
	if(RSDT != 0)
		return 0;
	uint32_t ebda_point = (int)(*(uint16_t *)(EBDA_P_OFF)) << 4;
	mprint("EBDA found at 0x%x", ebda_point);
	uint64_t* _sig;
	for(_sig = ebda_point & (uint64_t)(~0xF); _sig < 0x100000; _sig += 2)
		if(*_sig == 0x2052545020445352) // "RSD PTR "
		{
			RSDP = _sig;
			break;
		}

	if(RSDP == 0)
	{
		mprint("error: RSDT pointer not found!");
		return -1;
	}

	RSDT = RSDP->fp.rsdt_off;
	mprint("RSDT found at 0x%x", RSDP->fp.rsdt_off);
	mprint("ACPI rev: %d",RSDP->fp.rev);
	mprint("OEM: \"%6s\"",RSDP->fp.oem);
	return 0;
}