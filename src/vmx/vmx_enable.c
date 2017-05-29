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
	vmx_vmwrite(0x681e,3);
	const char* rsns[]=
	{
		"VMCALL executed in VMX root operation",
		"VMCLEAR with invalid physical address",
		"VMCLEAR with VMXON pointer",
		"VMLAUNCH with non-clear VMCS",
		"VMRESUME with non-launched VMCS",
		"VMRESUME after VMXOFF (VMXOFF and VMXON between VMLAUNCH and VMRESUME)",
		"VM entry with invalid control field(s)",
		"VM entry with invalid host-state field(s)",
		"VMPTRLD with invalid physical address",
		"VMPTRLD with VMXON pointer",
		"VMPTRLD with incorrect VMCS revision identifier",
		"VMREAD/VMWRITE from/to unsupported VMCS component",
		"VMWRITE to read-only VMCS component",
		"VMXON executed in VMX root operation",
		"VM entry with invalid executive-VMCS pointer",
		"VM entry with non-launched executive VMCS",
		"VM entry with executive-VMCS pointer not VMXON pointer (when attempting to deactivate the dual-monitor treatment of SMIs and SMM)",
		"VMCALL with non-clear VMCS (when attempting to activate the dual-monitor treatment of SMIs and SMM)",
		"VMCALL with invalid VM-exit control fields",
		"VMCALL with incorrect MSEG revision identifier (when attempting to activate the dual-monitor treatment of SMIs and SMM)",
		"VMXOFF under dual-monitor treatment of SMIs and SMM",
		"VMCALL with invalid SMM-monitor features (when attempting to activate the dual-monitor treatment of SMIs and SMM)",
		"VM entry with invalid VM-execution control fields in executive VMCS (when attempting to return from SMM)",
		"VM entry with events blocked by MOV SS.",
		"Invalid operand to INVEPT/INVVPID."
	};
	if(!vmx_vmlaunch())
		printf("vmlaunch successful");
	else
	{
		uint64_t reason = vmx_vmread(0x681e);
		printf("vmlaunch: VMFail\nReason #%x: ",reason);
		printf("%s\n", rsns[reason]);
	}
	return 0;
}

int vmx_vmwrite(uint64_t vmcs_id, uint64_t value)
{
	char err;
	asm("vmwrite %1, %0; setna %2"
		: "=r"(vmcs_id), "=r"(value), "=r"(err));
	return err ? -1 : 0;
}

int vmx_vmlaunch()
{
	char err;
	asm("vmlaunch; setna %0"
		: "=r"(err));
	return err ? -1 : 0;
}

uint64_t vmx_vmread(uint64_t vmcs_id)
{
	uint64_t value = 0;
	char err_carry, err_zero;
	asm volatile("xchg %bx, %bx");
	asm volatile("vmread %1, %0; setnae %2; sete %3"
		: "=r"(vmcs_id), "=r"(value), "=r"(err_carry), "=r"(err_zero));
	return value;
}
