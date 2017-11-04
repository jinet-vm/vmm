#include <jinet/apic.h>
#include <jinet/madt.h>
#include <jinet/irq.h>
#include <stdint.h>

#define LAPIC_ID *(lapic+LAPIC_ID_R_OFF*REG_S)
#define EOIR *(lapic+EOIR_N)
#define SVR *(lapic+SVR_N)

#define APIC_ENABLE (1 << 8)

#define REG_S 0x10

#define TPR 0x8
#define LDR 0xD
#define DFR 0xE
#define SVR 0xF

static void* lapic;

// checkout https://github.com/pdoane/osdev/blob/master/intr/local_apic.c

void lapic_setup()
{
	//pic_disable();
	lapic = madt_lapic_base();
	//printf("LAPIC regs at %08x\n", lapic);
	// uint32_t* reg = lapic+SVR_N*REG_S;
	// //printf("reg at %08x\n", reg);
	// *reg = (*reg) | APIC_ENABLE;
	// spurrious interrupt vector register
	lapic_reg_write(TPR, 0); 
	lapic_reg_write(DFR, 0xffffffff);
	lapic_reg_write(LDR, 0x01000000);
	lapic_reg_write(SVR, 0x1FF);
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
	//asm("xchg %bx, %bx");
	lapic_reg_write(0xB,0);
}