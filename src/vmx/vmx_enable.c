#include <kernel/vmx.h>
#include <kernel/msr.h>
#include <kernel/printf.h>
#define VMCS 0xffff800000010000

int size;

void vmx_on(void* paddr)
{
	asm("vmxon %0"
		:"=m"(paddr));
}

void vmx_crinit()
{
	// I HATE INLINE AT&T ASM!
	// I need it though.
	// todo: why multiline-inline-assembly needs '\n' ?
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
	printf("VMX revision: 0x%08x\n", bmsr & 0xffffffff);
	size = (bmsr >> 32) & 0x1fff;
	printf("VMCS size: 0x%d\n",size);
	if(size == 0) size = 1024;
	int* rev = 0xffff800000010000;
	*rev = bmsr & 0xffffffff;
	uint64_t* addr = 0xffff800000010000;
	*addr = 0x410000;
	vmx_on(0x410000);
	return 0;
}