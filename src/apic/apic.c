#include <kernel/apic.h>
#include <kernel/madt.h>
#include <kernel/irq.h>
#include <stdint.h>

// living in a magical world
// where the first 4 GiB are
// identity mapped. A bright
// but quite impossible pic.

// todo: fix it

#define LAPIC_ID *(lapic+LAPIC_ID_R_OFF*REG_S)
#define EOIR *(lapic+EOIR_N)
#define SVR *(lapic+SVR_N)

#define APIC_ENABLE 1 << 8

#define REG_S 0x10

static void* lapic;

void lapic_setup()
{
	pic_disable();
	asm("xchg %bx, %bx");
	lapic = madt_lapic_base();
	//printf("LAPIC regs at %08x\n", lapic);
	uint32_t* reg = lapic+SVR_N*REG_S;
	//printf("reg at %08x\n", reg);
	*reg = (*reg) | APIC_ENABLE;
}

uint32_t lapic_reg_read(int n)
{
	uint32_t* reg = lapic+n*REG_S;
	return *reg;
}

void lapic_reg_write(int n, uint32_t val)
{
	uint32_t* reg = lapic+n*REG_S;
	*reg = val;
}

// todo: whaaaaat?
void lapic_eoi_send()
{
	lapic_reg_write(0xB,0);
}