#include <kernel/vmx.h>
#include <kernel/msr.h>
#include <kernel/printf.h>
#include <kernel/debug.h>
#include <kernel/regs.h>

// TODO: stop being stupid
#define VMCS_L 0xffff800000010000
#define VMCS_P 0x410000

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


void virt_crinit()
{
	// I HATE INLINE AT&T ASM!
	// I need it though.
	// todo: why does multiline-inline-assembly need '\n' ?
	asm("movq %%cr4, %%rax\n"
		"bts $13, %%rax\n"
		"movq %%rax, %%cr4\n"
		: :	:"%rax"
		);
	//cr0_set(cr0_get() | (1 << 5));
	//cr0_get();
	asm("movq %%cr0, %%rax\n"
		"bts $5, %%rax\n"
		"movq %%rax, %%cr0\n"
		: :	:"%rax"
		);
}

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
		"",
		"VMXON executed in VMX root operation",
		"VM entry with invalid executive-VMCS pointer",
		"VM entry with non-launched executive VMCS",
		"VM entry with executive-VMCS pointer not VMXON pointer (when attempting to deactivate the dual-monitor treatment of SMIs and SMM)",
		"VMCALL with non-clear VMCS (when attempting to activate the dual-monitor treatment of SMIs and SMM)",
		"VMCALL with invalid VM-exit control fields",
		"",
		"VMCALL with incorrect MSEG revision identifier (when attempting to activate the dual-monitor treatment of SMIs and SMM)",
		"VMXOFF under dual-monitor treatment of SMIs and SMM",
		"VMCALL with invalid SMM-monitor features (when attempting to activate the dual-monitor treatment of SMIs and SMM)",
		"VM entry with invalid VM-execution control fields in executive VMCS (when attempting to return from SMM)",
		"VM entry with events blocked by MOV SS.",
		"",
		"Invalid operand to INVEPT/INVVPID."
};

char* vmx_reason()
{
	uint64_t reason = vmx_vmread(0x4400);
	if(reason > 28) reason = 0;
	return rsns[reason];
}

int virt_init()
{
	uint32_t vmcs_size, revision;

	// checking vmx
	if(!vmx_check()) return -1;
	printf("VMX supported!\n");

	// cr0 & cr4 init
	virt_crinit();

	// IA32_VMX_BASIC: getting revision
	uint64_t vmxBasic =msr_get(IA32_VMX_BASIC);
	vmcs_size = (vmxBasic >> 32) & 0x1fff;
	revision = vmxBasic & 0x7fffffff;
	printf("VMX revision: 0x%08x\n", revision);
	printf("VMCS size: 0x%d\n",vmcs_size);

	// vmxon region
	uint32_t* rev= VMCS_L;
	*rev = revision;

	// setting IA32_FEATURE_CONTROL
	uint64_t ifc = msr_get(IA32_FEATURE_CONTROL);
	printf("IA32_FEATURE_CONTROL: 0x%x%x\n", ifc >> 32, ifc);
	if(ifc % 2 == 0)
	{
		ifc |= (1 << 0) | (1 << 1) | (1 << 2); // locked!
		msr_set(IA32_FEATURE_CONTROL, ifc);
	}

	// vmxon
	if(!vmx_vmxon(VMCS_P))
		printf("vmxon successful\n");
	else
	{
		printf("vmxon: VMFailInvalid\n");
		return -1;
	}

	// vmcs: setting revision
	rev = VMCS_L+0x1000;
	*rev = revision;

	// vmptrld: loading vmcs pointer
	if(!vmx_vmptrld(VMCS_P+0x1000))
		printf("vmptrld successful\n");
	else
	{
		printf("vmptrld: VMFailInvalid (no active VMCS so far)\n");
		return -1;
	}
	
	// >> managing vmx controls
	// note: UL suffix required to become unsigned long
	char Truth = 0; // or Consequences
	if((1UL << 33) & msr_get(IA32_VMX_BASIC)) Truth = 1;

	// pin-based
	{
		uint64_t pinb;
		uint32_t zero, one, pinbvm;
		if(Truth)
		{
			// todo:
			//printf("forming vmx pin-based controls from IA32_VMX_TRUE_PINBASED_CTLS\n");
			pinb = msr_get(IA32_VMX_TRUE_PINBASED_CTLS);
		}
		else
		{
			// todo: understand it? see 3D, page 183
			//printf("forming vmx pin-based controls from IA32_VMX_PINBASED_CTLS\n");
			pinb = msr_get(IA32_VMX_PINBASED_CTLS);
			zero = pinb & 0xffffffff, one = pinb >> 32;
			printf("0x%x & 0x%x\n", zero, one);
			pinbvm |= zero;
		}

		if(!vmx_vmwrite(VMX_PINBASED_CTLS_D, pinbvm))
			printf("vmwrite: OK!\n");
		else
			printf("vmwrite: VMFail\nReason: %s", vmx_reason());
	}
	
	// proc-based
	{
		uint64_t procb;
		uint32_t zero, one, procbvm;
		procb = msr_get(IA32_VMX_PROCBASED_CTLS);
		zero = procb & 0xffffffff, one = procb >> 32;
		printf("0x%x & 0x%x\n", zero, one);
		procbvm |= ~zero;
		procbvm |= one;
		if(!vmx_vmwrite(VMX_PROCBASED_CTLS_D, procb))
			printf("vmwrite: OK!\n");
		else
			printf("vmwrite: VMFail\nReason: %s", vmx_reason());
	}
	
	// vm-exit
	{
		uint64_t vmex;
		uint32_t zero, one, vmexvm;
		vmex = msr_get(IA32_VMX_EXIT_CTLS);
		zero = vmex & 0xffffffff, one = vmex >> 32;
		printf("0x%x & 0x%x\n", zero, one);

		vmexvm = 0;
		vmexvm |= zero;
		vmexvm |= (1 << 9); // Host address-space size

		//printf("vmexit_ctrls: %08x\n", pinbvm);

		if(!vmx_vmwrite(VMX_VMEXIT_CTLS_D, vmexvm))
			printf("vmwrite: OK!\n");
		else
			printf("vmwrite: VMFail\nReason: %s", vmx_reason());
	}

	// vm-entry
	{
		uint64_t vmen;
		uint32_t zero, one, vmenvm;
		vmen = msr_get(IA32_VMX_ENTRY_CTLS);
		zero = vmen & 0xffffffff, one = vmen >> 32;
		printf("0x%x & 0x%x\n", zero, one);
		vmenvm = 0;
		vmenvm |= zero;
		//vmenvm |= one;

		if(!vmx_vmwrite(VMX_VMENTRY_CTLS_D, vmenvm))
			printf("vmwrite: OK!\n");
		else
			printf("vmwrite: VMFail\nReason: %s", vmx_reason());
	}

	// cr0 bits
	{
		uint32_t zero = msr_get(IA32_VMX_CR0_FIXED0);
		uint32_t one = msr_get(IA32_VMX_CR0_FIXED1);
		uint64_t vmcr0 = zero;

		if(!vmx_vmwrite(VMX_GUEST_CR0_N, vmcr0))
			printf("vmwrite: OK!\n");
		else
			printf("vmwrite: VMFail\nReason: %s", vmx_reason());
	}

	// host cr0
	{
		if(!vmx_vmwrite(VMX_HOST_CR0_N, msr_get(IA32_VMX_CR0_FIXED1)))
			printf("vmwrite: OK!\n");
		else
			printf("vmwrite: VMFail\nReason: %s", vmx_reason());
	}

	// host cr4
	{
		if(!vmx_vmwrite(VMX_HOST_CR4_N, msr_get(IA32_VMX_CR4_FIXED1)))
			printf("vmwrite: OK!\n");
		else
			printf("vmwrite: VMFail\nReason: %s", vmx_reason());
	}

	// host cs
	{
		if(!vmx_vmwrite(VMX_HOST_CS_W, cs_get()))
			printf("vmwrite: OK!\n");
		else
			printf("vmwrite: VMFail\nReason: %s", vmx_reason());
	}

	// host tr
	{
		if(!vmx_vmwrite(VMX_HOST_TR_W, 0))
			printf("vmwrite: OK!\n");
		else
			printf("vmwrite: VMFail\nReason: %s", vmx_reason());
	}

	// vm_launch
	if(!vmx_vmlaunch())
		printf("vmlaunch successful");
	else
		printf("vmlaunch: VMFail\nReason: %s", vmx_reason());
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