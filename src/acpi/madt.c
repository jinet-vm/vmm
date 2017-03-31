#include <kernel/madt.h>
#include <kernel/acpi.h>
#include <kernel/printf.h>
#include <stdint.h>

// http://www.acpi.info/DOWNLOADS/ACPI_5_Errata A.pdf - 5.2.12.2 and upper

#define TYPE_LAPIC 0
#define TYPE_IOAPIC 1

struct madt
{
	struct sdt_header h;
	uint32_t lapic_off;
	uint32_t flags;
	char data[];
} __attribute__ ((packed));

struct lapic
{
	uint8_t type; // = 0
	uint8_t length; // = 8
	uint8_t acpi_id;
	uint8_t apic_id;
	uint32_t flags;
} __attribute__ ((packed));

struct ioapic
{
	uint8_t type; // = 1
	uint8_t length; // = 12
	uint8_t ioapic_id;
	uint8_t reserved;
	uint32_t ioapic_off;
	uint32_t gsi_base;
} __attribute__ ((packed));

// there're other stuff we'll ignore

static struct madt* MADT;

void detect_madt()
{
	if(MADT != 0)
		return;
	detect_rsdt();
	MADT = find_sdt("APIC");
}

uint32_t madt_lapic_base()
{
	return MADT->lapic_off;
}

void detect_cpu_topology()
{
	detect_madt();
	printf("MADT sig: %.4s; len: %d bytes\n", MADT->h.sig, MADT->h.length);
	uint32_t madt_beg = MADT;
	uint32_t madt_end = madt_beg+MADT->h.length;
	printf("MADT at [0x%08x; 0x%08x]\n",madt_beg,madt_end);
	printf("LAPIC base: 0x%08x\n",MADT->lapic_off);
	char* p = MADT->data;
	struct lapic* cl;
	struct ioapic* ioa;
	while(p < madt_end)
	{
		switch(*p)
		{
			case TYPE_LAPIC:
				cl = p;
				printf("LAPIC detected; APIC ID: %xh                \n", cl->apic_id);
			break;
			
			case TYPE_IOAPIC:
				ioa = p;
				printf("IOAPIC detected; IOAPIC ID: %xh               \n", ioa->ioapic_off);
			break;

			default:
				ioa = p;
				printf("Unknown entry; type: %d, length: %d bytes      \n", ioa->type, ioa->length);
			break;
		}
		ioa = p; // a hacker solution
		uint8_t length = ioa -> length;
		if(length == 0) // scary stuff
			return;
		p += length;
	}
}