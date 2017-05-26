#include <kernel/vmx.h>
#include <kernel/msr.h>
#include <kernel/printf.h>

// TODO: stop being stupid
#define VMCS_L 0xffff800000010000
#define VMCS_P 0x410000

int size;

void vmx_vmxon(void* paddr)
{
	asm("vmxon %0"
		:"=m"(paddr));
}

void vmx_vmptrld(unsigned long vmcs)
{
unsigned char err;

	__asm__ volatile ("vmptrld %1; setna %0"
		: "=r"(err)
		: "m"(vmcs)
		: "memory", "cc");
	return err ? -1 : 0;
}


void vmx_crinit()
{
	// I HATE INLINE AT&T ASM!
	// I need it though.
	// todo: why does multiline-inline-assembly need '\n' ?
	asm("movq %%cr4, %%rax\n"
		"bts $13, %%rax\n"
		"movq %%rax, %%cr4\n"
		: :	:"%rax"
		);
	asm("movq %%cr0, %%rax\n"
		"bts $5, %%rax\n"
		"movq %%rax, %%cr0\n"
		: :	:"%rax"
		);
}

int vmx_init()
{
	if(!vmx_check()) return -1;
	printf("VMX supported!\n");
	vmx_crinit();
	uint64_t bmsr=0;
	msr_get(0x480,&bmsr,4+(char*)(&bmsr));
	printf("VMX revision: 0x%08x\n", bmsr & 0x8fffffff);
	size = (bmsr >> 32) & 0x1fff;
	printf("VMCS size: 0x%d\n",size);
	// vmxon region:
	int* rev = VMCS_L;
	*rev = bmsr & 0x8fffffff;
	vmx_vmxon(VMCS_P);
	printf("vmxon succesful\n");
	// vmcs:
	rev += 0x1000;
	*rev = bmsr & 0x8fffffff;
	vmx_vmptrld(VMCS_P+0x1000);
	return 0;
}

void vmwrite(uint64_t vmcs_id, uint64_t value)
{
	asm("vmwrite %1, %0"
		: "=r"(vmcs_id), "=r"(value));
}