#include <kernel/vmx.h>
#include <kernel/printf.h>
#define VMCS 0xffff800000010000

int vmx_init()
{
	if(!vmx_check()) return -1;
	printf("VMX supported!\n");
	uint64_t bmsr = vmx_bmsr();
	printf("VMX revision: 0x%08x\n", bmsr & 0xffffffff);
	printf("VMCS size: 0x%08x\n",bmsr >> 32);
	return 0;
}