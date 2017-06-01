#include <kernel/vmx.h>
#include <kernel/msr.h>
#include <kernel/printf.h>
#include <kernel/debug.h>

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
	uint64_t bmsr=msr_get(0x480);
	printf("VMX revision: 0x%08x\n", bmsr & 0x7fffffff);
	size = (bmsr >> 32) & 0x1fff;
	printf("VMCS size: 0x%d\n",size);
	// vmxon region:
	int* rev = VMCS_L;
	*rev = bmsr & 0x7fffffff;
	uint64_t ifc = msr_get(IA32_FEATURE_CONTROL);
	printf("IA32_FEATURE_CONTROL: 0x%x%x\n", ifc >> 32, ifc);
	if(ifc % 2 == 0)
	{
		ifc |= (1 << 0) | (1 << 1) | (1 << 2); // locked!
		msr_set(IA32_FEATURE_CONTROL, ifc);
	}
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
	mbp;
	vmx_vmwrite(0x681e,3);
	const char* rsns[]=
	{
		"",
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
	//return 0;
	uint64_t pinb;
	uint32_t zero, one;
	if(msr_get(IA32_VMX_BASIC) & (1 << 55))
	{
		printf("forming vmx pin-based controls from IA32_VMX_TRUE_PINBASED_CTLS\n");
		pinb = msr_get(IA32_VMX_TRUE_PINBASED_CTLS);
	}
	else
	{
		// todo:
		// i can't understand it; see 3D, page 183
		printf("forming vmx pin-based controls from IA32_VMX_PINBASED_CTLS\n");
		pinb = msr_get(IA32_VMX_PINBASED_CTLS);
		zero = pinb & 0xffffffff, one = pinb >> 32;
		printf("0x%x & 0x%x\n", zero, one);
		uint32_t pinbvm = 0;
		pinbvm |= ~zero;
		pinbvm |= one;
	}

	if(!vmx_vmwrite(0x4000, pinb))
		printf("vmwrite: OK!\n");
	else
	{
		uint64_t reason = vmx_vmread(0x4400);
		printf("vmwrite: VMFail\nReason #%x: ",reason);
		printf("%s\n", rsns[reason]);
	}

	// todo:
	// i can't understand it; see 3D, page 183
	//printf("forming vmx pin-based controls from IA32_VMX_PINBASED_CTLS\n");
	pinb = msr_get(IA32_VMX_PROCBASED_CTLS);
	zero = pinb & 0xffffffff, one = pinb >> 32;
	printf("0x%x & 0x%x\n", zero, one);
	uint32_t pinbvm = 0;
	pinbvm |= ~zero;
	pinbvm |= one;

	if(!vmx_vmwrite(0x4002, pinb))
		printf("vmwrite: OK!\n");
	else
	{
		uint64_t reason = vmx_vmread(0x4400);
		printf("vmwrite: VMFail\nReason #%x: ",reason);
		printf("%s\n", rsns[reason]);
	}

		// todo:
	// i can't understand it; see 3D, page 183
	//printf("forming vmx pin-based controls from IA32_VMX_PINBASED_CTLS\n");
	pinb = msr_get(IA32_VMX_EXIT_CTLS);
	zero = pinb & 0xffffffff, one = pinb >> 32;
	printf("0x%x & 0x%x\n", zero, one);
	pinbvm = 0;
	pinbvm |= ~zero;
	pinbvm |= one;

	if(!vmx_vmwrite(0x400C, pinb))
		printf("vmwrite: OK!\n");
	else
	{
		uint64_t reason = vmx_vmread(0x4400);
		printf("vmwrite: VMFail\nReason #%x: ",reason);
		printf("%s\n", rsns[reason]);
	}


	pinb = msr_get(IA32_VMX_ENTRY_CTLS);
	zero = pinb & 0xffffffff, one = pinb >> 32;
	printf("0x%x & 0x%x\n", zero, one);
	pinbvm = 0;
	pinbvm |= ~zero;
	pinbvm |= one;

	if(!vmx_vmwrite(0x4012, pinb))
		printf("vmwrite: OK!\n");
	else
	{
		uint64_t reason = vmx_vmread(0x4400);
		printf("vmwrite: VMFail\nReason #%x: ",reason);
		printf("%s\n", rsns[reason]);
	}



	//printf("0x%x%x\n", pinb >> 32, pinb);
	// uint64_t pinb = msr_get(IA32_VMX_PINBASED_CTLS);
	// uint32_t zero = pinb & 0xffffffff, one = pinb >> 32;
	if(!vmx_vmlaunch())
		printf("vmlaunch successful");
	else
	{
		uint64_t reason = vmx_vmread(0x4400);
		printf("vmlaunch: VMFail\nReason #%x: ",reason);
		printf("%s\n", rsns[reason]);
	}
}

int vmx_vmwrite(uint64_t vmcs_id, uint64_t value)
{
	char err;
	asm("vmwrite %2, %1; setna %0"
		: "=r"(err)
		: "r"(vmcs_id), "r"(value));
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
	asm volatile("xchg %bx, %bx");
	asm volatile("vmread %0, %1"
		: "=r"(value)
		: "r"(vmcs_id));
	return value;
}