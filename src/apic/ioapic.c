#include <kernel/ioapic.h>
#include <kernel/madt.h>

#define REG_S 0x8

static uint32_t* ioregsel;
static uint32_t* iowin;

#define IRS_OFF 0
#define IW_OFF 0x10

#define IOREDTBL_OFF 0x10

void ioapic_setup()
{
	uint32_t ioapic = madt_ioapic_base();
	ioregsel = ioapic + IRS_OFF;
	iowin = ioapic + IW_OFF;
}

uint32_t ioapic_reg_read(uint8_t n)
{
	*ioregsel = n;
	return *iowin;
}

void ioapic_reg_write(uint8_t n, uint32_t val)
{
	*ioregsel = n;
	*iowin = val;
}

// todo: enums?

void ioapic_set_gate(uint8_t n, uint8_t intvec, uint8_t delmod, uint8_t destmod, uint8_t intpol, uint8_t trigmod, uint8_t intmask, uint8_t dest)
{
	if(n > 23) return;
	uint8_t low = 0, high = 0;
	low |= intvec << 0;
	low |= (delmod & 7) << 8;
	low |= (destmod & 1) << 11;
	low |= (intpol & 1) << 13;
	low |= (trigmod & 1) << 15;
	low |= (intmask & 1)  << 16;
	high |= dest << 24;
	ioapic_reg_write(IOREDTBL_OFF+n*2,low);
	ioapic_reg_write(IOREDTBL_OFF+n*2+1,high);
}