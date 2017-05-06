#include <kernel/vmx.h>
#include <kernel/printf.h>
#define VMCS 0xffff800000010000

int size;

int vmx_init()
{
	if(!vmx_check()) return -1;
	printf("VMX supported!\n");
	uint64_t bmsr = vmx_bmsr();
	printf("VMX revision: 0x%08x\n", bmsr & 0xffffffff);
	size = (bmsr >> 32) & 0x1fff;
	printf("VMCS size: 0x%d\n",size);
	if(size == 0) size = 1024;
	int* rev = 0xffff800000010000;
	*rev = bmsr & 0xffffffff;
	int* addr = 0xffff800000020000;
	*addr = 0x410000;
	vmx_on(&addr);
	return 0;
}