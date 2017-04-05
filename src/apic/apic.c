#include <kernel/apic.h>
#include <kernel/madt.h>
#include <kernel/irq.h>
#include <stdint.h>

// living in a magical world
// where the first 4 GiB are
// identity mapped. A bright
// but quite impossible pic.

struct areg
{
	uint64_t a;
	uint64_t b;
};

// todo: fix it

#define REG_S 0x10

#define LAPIC_ID_N 0x2
#define LAPIC_ID *(lapic+LAPIC_ID_R_OFF*REG_S)

#define EOIR_N 0xB
#define EOIR *(lapic+EOIR_N)

#define SVR_N 0xF
#define SVR *(lapic+SVR_N)

#define APIC_ENABLE 1 << 8

static void* lapic;

void lapic_setup()
{
	pic_disable();
	asm("xchg %bx, %bx");
	lapic = madt_lapic_base();
	//printf("FUCKKFUFUCUCUCK\n");
	//printf("LAPIC regs at %08x\n", lapic);
	struct areg* reg = lapic;
	reg += SVR_N;
	//printf("reg at %08x", reg);
	reg->a |= APIC_ENABLE;
}