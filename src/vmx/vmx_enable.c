#include <kernel/vmx.h>
#include <kernel/msr.h>
#include <kernel/printf.h>

// TODO: stop being stupid
#define VMCS_L 0xffff800000010000
#define VMCS_P 0x410000

int size;

int vmx_vmxon(uint64_t paddr)
{
	unsigned char err;
	asm volatile ("vmxon %1; setna %0"
		: "=r"(err)
		: "m"(paddr)
		: "memory", "cc");
	return err ? -1 : 0;
}

int vmx_vmptrld(unsigned long vmcs)
{
	unsigned char err;
	asm volatile ("vmptrld %1; setna %0"
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
	printf("VMX revision: 0x%08x\n", bmsr & 0x7fffffff);
	size = (bmsr >> 32) & 0x1fff;
	printf("VMCS size: 0x%d\n",size);
	// vmxon region:
	int* rev = VMCS_L;
	*rev = bmsr & 0x7fffffff;
	if(!vmx_vmxon(VMCS_P))
		printf("vmxon successful\n");
	else
	{
		printf("vmxon: VMFailInvalid\n");
		return -1;
	}
	// vmcs:
	rev = VMCS_L+0x1000;
	*rev = bmsr & 0x7fffffff;
	if(!vmx_vmptrld(VMCS_P+0x1000))
		printf("vmptrld successful\n");
	else
	{
		printf("vmptrld: VMFailInvalid (no active VMCS so far)\n");
		return -1;
	}
	//vmwrite(0x681e,0x7c00);
	if(!vmlaunch())
		printf("vmlaunch successful");
	else
		printf("vmlaunch: VMFail");
	return 0;
}

int vmwrite(uint64_t vmcs_id, uint64_t value)
{
	char err;
	asm("vmwrite %1, %0; setna %2"
		: "=r"(vmcs_id), "=r"(value), "=r"(err));
	return err ? -1 : 0;
}

int vmlaunch()
{
	char err;
	asm("vmlaunch; setna %0"
		: "=r"(err));
	return err ? -1 : 0;
}