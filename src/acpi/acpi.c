#include <kernel/acpi.h>
#include <stdint.h>

#define EBDA_P_OFF 0x40E

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

static struct rsdp_2_0* RSDP;
static struct rsdt* RSDT = 0;

struct sdt_header* find_sdt(char* sig) // living in RSDT 32-bit world. THIS SUCKS!
{
	int ent = (RSDT->h.length-sizeof(RSDT->h))/sizeof(RSDT->sdt_p[0]); // TODO: XSDT!
	int* sig1 = sig; // 4-byte char; REMIND YOU OF ANYBODY?
	for(int i = 0; i<ent; i++)
	{
		struct sdt_header* sh = RSDT->sdt_p[i];
		int* sig2 = sh;
		if(*sig1 == *sig2) // found!
			return RSDT->sdt_p[i];
	}
}

int detect_rsdt()
{
	if(RSDT != 0)
		return 0;
	const char rsdp_sig[8] = "RSD PTR ";
	uint16_t* ebdaba_point = EBDA_P_OFF; // EBDA base address
	uint32_t ebda_point = (int)(*ebdaba_point) << 4;
	printf("EBDA: 0x%x\n", ebda_point);
	char* i;
	for(i = ebda_point; i < 0x100000; i++)
	{
		//printf("add = %x\n", *i);
		int j;
		for(j = 0; j<8; j++)
			if(*(i+j) != rsdp_sig[j])
				break;
		if(j != 8) continue;
		break;
	}
	RSDP = i;
	printf("RSDT: 0x%x\n", RSDP->fp.rsdt_off);
	RSDT = RSDP->fp.rsdt_off;
	printf("ACPI rev: %d\n",RSDP->fp.rev);
	printf("OEM: \"%6s\"\n",RSDP->fp.oem);
	return 0;
}