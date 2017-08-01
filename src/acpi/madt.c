#include <kernel/madt.h>
#include <kernel/acpi.h>
#include <kernel/module.h>
#include <stdint.h>

// http://www.acpi.info/DOWNLOADS/ACPI_5_Errata A.pdf - 5.2.12.2 and upper

// todo: MADT probe function!

MODULE("ACPIMADT");

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

int madt_probe(void* tbl)
{
	MADT = tbl;
	madt_topology_detect();
}

uint32_t madt_lapic_base()
{
	return MADT->lapic_off;
}

static uint32_t IOAPIC_ADDR = 0xFEC00000; // default value

uint32_t madt_ioapic_base()
{
	return IOAPIC_ADDR;
}

void madt_topology_detect()
{
	mprint("MADT sig: %.4s; len: %d bytes", MADT->h.sig, MADT->h.length);
	uint32_t madt_beg = MADT;
	uint32_t madt_end = madt_beg+MADT->h.length;
	mprint("MADT at [0x%08x; 0x%08x]",madt_beg,madt_end);
	mprint("LAPIC base: 0x%08x",MADT->lapic_off);
	char* p = MADT->data;
	struct lapic* cl;
	struct ioapic* ioa;
	while(p < madt_end)
	{
		switch(*p)
		{
			case TYPE_LAPIC:
				cl = p;
				#ifdef MADT_OUTPUT
					mprint("LAPIC detected; APIC ID: %xh", cl->apic_id);
				#endif
			break;
			
			case TYPE_IOAPIC:
				ioa = p;
				#ifdef MADT_OUTPUT
				mprint("IOAPIC detected; IOAPIC ID: 0x%xoffset: 0x%x", ioa->ioapic_id, ioa->ioapic_off);
				#endif
				IOAPIC_ADDR = ioa->ioapic_off;
			break;

			default:
				ioa = p;
				#ifdef MADT_OUTPUT
					mprint("Unknown entry; type: %d, length: %d bytes", ioa->type, ioa->length);
				#endif
			break;
		}
		ioa = p; // a hacker solution
		uint8_t length = ioa -> length;
		if(length == 0) // scary stuff
			return;
		p += length;
	}
}