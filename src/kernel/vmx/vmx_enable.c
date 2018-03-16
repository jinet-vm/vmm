
#include <jinet/vmx.h>
#include <jinet/msr.h>
#include <jinet/tty.h>
#include <jinet/debug.h>
#include <jinet/regs.h>
#include <jinet/gdt.h>
#include <jinet/idt.h>
#include <jinet/memory.h>
#include <jinet/module.h>
#include <jinet/paging.h>
#include <jinet/printf.h>
#include <jinet/vmaddr.h>
#include <jinet/ept.h>
// TODO: stop being stupid
// #define VMCS_L 0xffff800000010000
// #define VMCS_P 0x410000

void* VMCS_L;
uint64_t VMCS_P;

MODULE("VMX_ENABLE");

int vmx_vmxon(uint64_t paddr)
{
	unsigned char err;
	asm volatile ("vmxon %1; setna %0"
		: "=r"(err)
		: "m"(paddr)
		: "memory", "cc");
	return err ? -1 : 0;
}

int vmxon(uint64_t paddr, char debug)
{
	if(!debug) return vmx_vmxon(paddr);
	int vmxon_res = vmx_vmxon(VMCS_P);
	if(!vmxon_res)
	{
		mprint("vmxon successful");
		return 0;
	}
	else
	{
		mprint("vmxon: VMFailInvalid");
		return vmxon_res;
	}
}

int vmx_vmptrld(uint64_t vmcs)
{
	unsigned char err;
	asm volatile ("vmptrld %1; setna %0"
		: "=r"(err)
		: "m"(vmcs)
		: "memory", "cc");
	return err ? -1 : 0;
}

int vmx_vmclear(uint64_t vmcs)
{
	unsigned char err;
	asm volatile ("vmclear %1; setna %0"
		: "=r"(err)
		: "m"(vmcs)
		: "memory", "cc");
	return err ? -1 : 0;
}

int vmptrld(uint64_t vmcs, char debug)
{
	if(!debug) return vmx_vmptrld(vmcs);
	int vmxon_res = vmx_vmptrld(vmcs);
	if(!vmxon_res)
	{
		mprint("vmptrld successful");
		return 0;
	}
	else
	{
		mprint("vmptrld: VMFailInvalid");
		return vmxon_res;
	}
}

int vmclear(uint64_t vmcs, char debug)
{
	if(!debug) return vmx_vmclear(vmcs);
	int vmxon_res = vmx_vmclear(vmcs);
	if(!vmxon_res)
	{
		mprint("vmclear successful");
		return 0;
	}
	else
	{
		mprint("vmclear: VMFailInvalid");
		return vmxon_res;
	}
}

void virt_crinit()
{
	// I HATE INLINE AT&T ASM!
	// I need it though.
	// todo: why does multiline-inline-assembly need '' ?
	asm("movq %%cr4, %%rax\n"
		"bts $13, %%rax\n"
		"movq %%rax, %%cr4"
		: :	:"%rax"
		);
	//cr0_set(cr0_get() | (1 << 5));
	//cr0_get();
	asm("movq %%cr0, %%rax\n"
		"bts $5, %%rax\n"
		"movq %%rax, %%cr0"
		: :	:"%rax"
		);
}



char* virt_reason()
{

	static const char* rsns[]=
	{
		"Reserved",
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
		"Reserved",
		"VMXON executed in VMX root operation",
		"VM entry with invalid executive-VMCS pointer",
		"VM entry with non-launched executive VMCS",
		"VM entry with executive-VMCS pointer not VMXON pointer (when attempting to deactivate the dual-monitor treatment of SMIs and SMM)",
		"VMCALL with non-clear VMCS (when attempting to activate the dual-monitor treatment of SMIs and SMM)",
		"VMCALL with invalid VM-exit control fields",
		"Reserved",
		"VMCALL with incorrect MSEG revision identifier (when attempting to activate the dual-monitor treatment of SMIs and SMM)",
		"VMXOFF under dual-monitor treatment of SMIs and SMM",
		"VMCALL with invalid SMM-monitor features (when attempting to activate the dual-monitor treatment of SMIs and SMM)",
		"VM entry with invalid VM-execution control fields in executive VMCS (when attempting to return from SMM)",
		"VM entry with events blocked by MOV SS.",
		"Reserved",
		"Invalid operand to INVEPT/INVVPID."
	};
	uint64_t reason = vmx_vmread(0x4400);
	if(reason > 28) reason = 0;
	return rsns[reason];
}

#define VMX_DEBUG 1

static uint8_t exit_stack[1024];
static uint8_t vm1_stack[1024];
static uint8_t vm2_stack[1024];


asm("vm1_inside: .incbin \"bin/vm1.bin\"");
asm("vm2_inside: .incbin \"bin/vm2.bin\"");

extern void* vm1_inside;
extern void* vm2_inside;

extern void (*vmx_return)();

int virt_setup_vm();
uint32_t vmcs_size, revision;

typedef struct
{
	union
	{
		struct 
		{
			uint8_t type:4;
			uint8_t s:1;
			uint8_t dpl:2;
			uint8_t p:1;
			uint8_t _0:4;
			uint8_t avl:1;
			uint8_t l:1;
			uint8_t db:1;
			uint8_t g:1;
			uint8_t unuse:1;
			uint16_t _1:15;	
		} __attribute__((packed));
		uint32_t raw;
	}
} ar_t;

int virt_init()
{	
	mprint("test");
	VMCS_P = physmm_alloc(4);
	pg_map_reg(VMA_VMCS, VMCS_P, VMA_VMCS_SIZE);
	VMCS_L = VMA_VMCS;
	memcpy(0x7000lu, &vm1_inside, 0x100); // 'cause compatibility mode; btw, WHY?!
	memcpy(0x7100lu, &vm2_inside, 0x100); // 'cause compatibility mode; btw, WHY?!

	// checking vmx
	if(!vmx_check()) return -1;
	mprint("VMX supported!");

	// cr0 & cr4 init
	virt_crinit();

	// IA32_VMX_BASIC: getting revision
	uint64_t vmxBasic = msr_get(IA32_VMX_BASIC);
	vmcs_size = (vmxBasic >> 32) & 0x1fff;
	revision = vmxBasic & 0x7fffffff;
	mprint("VMX revision: 0x%08x", revision);
	mprint("VMCS size: 0x%d",vmcs_size);

	virt_setup_vm();
}

int virt_setup_vm()
{
	mprint("virt setupvm");
	// vmxon region
	uint32_t* rev= VMCS_L;
	*rev = revision;

	// setting IA32_FEATURE_CONTROL
	uint64_t ifc = msr_get(IA32_FEATURE_CONTROL);
	mprint("IA32_FEATURE_CONTROL: 0x%x%x", ifc >> 32, ifc);
	if(ifc % 2 == 0)
	{
		ifc |= (1 << 0) | (1 << 1) | (1 << 2); // locked!
		msr_set(IA32_FEATURE_CONTROL, ifc);
	}

	vmxon(VMCS_P, 1);


	// vmcs: setting revision
	rev = VMCS_L+0x1000;
	*rev = revision;
	// vmclear the vmcs
	mprint("test");
	vmclear(VMCS_P+0x1000, VMX_DEBUG);

	// vmptrld: loading vmcs pointer
	vmptrld(VMCS_P+0x1000, VMX_DEBUG);
	
	// >> managing vmx controls
	// note: UL suffix required to become unsigned long
	char Truth = 0; // or Consequences
	if((1UL << 33) & msr_get(IA32_VMX_BASIC)) Truth = 1;

	// pin-based
	{
		uint64_t pinb;
		uint32_t zero, one, pinbvm;
		// if(Truth)
		// {
		// 	// todo:
		// 	//mprint("forming vmx pin-based controls from IA32_VMX_TRUE_PINBASED_CTLS");
		// 	pinb = msr_get(IA32_VMX_TRUE_PINBASED_CTLS);
		// }
		// else
		// {
		// 	// todo: understand it? see 3D, page 183
		// 	//mprint("forming vmx pin-based controls from IA32_VMX_PINBASED_CTLS");
		// 	pinb = msr_get(IA32_VMX_PINBASED_CTLS);
		// 	zero = pinb & 0xffffffff, one = pinb >> 32;
		// 	mprint("0x%x & 0x%x", zero, one);
		// 	pinbvm |= zero;
		// }
		pinb = msr_get(IA32_VMX_PINBASED_CTLS);
		zero = pinb & 0xffffffff, one = pinb >> 32;
		mprint("0x%x & 0x%x", zero, one);
		pinbvm = one & ~(1 << 0);
		pinbvm &= ~(1 << 6);

		vmwrite(VMX_PINBASED_CTLS_D, pinbvm, VMX_DEBUG);
	}
	
	mprint("sec 1-set: %llx", msr_get(IA32_VMX_PROCBASED_CTLS2));
	// proc-based
	{
		uint64_t procb;
		uint32_t zero, one, procbvm;
		procb = msr_get(IA32_VMX_PROCBASED_CTLS) | (1 << 31); // enabling secondary controlss
		zero = procb & 0xffffffff, one = procb >> 32;
		mprint("0x%x & 0x%x", zero, one);
		procbvm |= ~zero;
		procbvm |= one;
		procbvm |= 1 << 31;
		mprint("IA32_VMX_PROCBASED_CTLS = %d", msr_get(IA32_VMX_PROCBASED_CTLS));
		mprint("IA32_VMX_PROCBASED_CTLS2 = %d", msr_get(IA32_VMX_PROCBASED_CTLS2));
		vmwrite(VMX_PROCBASED_CTLS_D, procb, VMX_DEBUG);
		vmwrite(VMX_SEC_PROCBASED_CTLS_D, (1 << 1) | (1 << 7), VMX_DEBUG); // ept
	}
	
	// vm-exit
	{
		uint64_t vmex;
		uint32_t zero, one, vmexvm;
		vmex = msr_get(IA32_VMX_EXIT_CTLS);
		zero = vmex & 0xffffffff, one = vmex >> 32;
		mprint("0x%x & 0x%x", zero, one);

		vmexvm = 0;
		vmexvm |= zero;
		vmexvm |= (1 << 9); // Host address-space size

		//mprint("vmexit_ctrls: %08x", pinbvm);
		vmwrite(VMX_VMEXIT_CTLS_D, vmexvm, VMX_DEBUG);
	}

	// vm-entry
	{
		uint64_t vmen;
		uint32_t zero, one, vmenvm;
		vmen = msr_get(IA32_VMX_ENTRY_CTLS);
		zero = vmen & 0xffffffff, one = vmen >> 32;
		mprint("0x%x & 0x%x", zero, one);
		vmenvm = 0;
		vmenvm |= zero;
		vmenvm |= (1 << 15); // load efer
		vmwrite(VMX_VMENTRY_CTLS_D, vmenvm, VMX_DEBUG);
	}

	// efer
	{
		vmwrite(VMX_GUEST_EFER_Q, 0, VMX_DEBUG);
	}

	// cr0 bits
	{
		// todo: are you sure? why not inverted?
		uint32_t zero = msr_get(IA32_VMX_CR0_FIXED0);
		uint32_t one = msr_get(IA32_VMX_CR0_FIXED1);
		uint64_t flex = ~zero & one;
		uint64_t vmcr0 = 0;
		//vmcr0 &= flex;
		vmcr0 |= zero;
		vmcr0 &= one;
		vmcr0 &= ~((1 << 0) | (1 << 31)); // if unrestricted guest is set, these bit are not checked
		mprint("vmcr0 = %08x", vmcr0);
		vmwrite(VMX_GUEST_CR0_N, vmcr0, VMX_DEBUG);
	}

	{
		// host seg regs
		vmwrite(VMX_HOST_CS_W, cs_get(), VMX_DEBUG);
		vmwrite(VMX_HOST_DS_W, ds_get(), VMX_DEBUG);
		vmwrite(VMX_HOST_ES_W, es_get(), VMX_DEBUG);
		vmwrite(VMX_HOST_GS_W, gs_get(), VMX_DEBUG);
		vmwrite(VMX_HOST_FS_W, fs_get(), VMX_DEBUG);
		vmwrite(VMX_HOST_SS_W, ss_get(), VMX_DEBUG);
	}

	//mprint("0x%x",lar(cs_get));

	{
		vmwrite(VMX_GUEST_CS_AR_D, (ar_t){.type = 3, .dpl = 0, .s = 1, .p = 1, .db = 0, .l = 0, .g = 0}.raw, VMX_DEBUG);
		vmwrite(VMX_GUEST_DS_AR_D, (ar_t){.type = 1, .dpl = 0, .s = 1, .p = 1, .db = 0, .l = 0, .g = 0}.raw, VMX_DEBUG);
		vmwrite(VMX_GUEST_ES_AR_D, (ar_t){.type = 1, .dpl = 0, .s = 1, .p = 1, .db = 0, .l = 0, .g = 0}.raw, VMX_DEBUG);
		vmwrite(VMX_GUEST_GS_AR_D, (ar_t){.type = 1, .dpl = 0, .s = 1, .p = 1, .db = 0, .l = 0, .g = 0}.raw, VMX_DEBUG);
		vmwrite(VMX_GUEST_FS_AR_D, (ar_t){.type = 1, .dpl = 0, .s = 1, .p = 1, .db = 0, .l = 0, .g = 0}.raw, VMX_DEBUG);
		vmwrite(VMX_GUEST_SS_AR_D, (ar_t){.type = 3, .dpl = 0, .s = 1, .p = 1, .db = 0, .l = 0, .g = 0}.raw, VMX_DEBUG);
	}
	// host cr0
	//vmwrite(VMX_HOST_CR0_N, msr_get(IA32_VMX_CR0_FIXED1), VMX_DEBUG);
	vmwrite(VMX_HOST_CR0_N, cr0_get(), VMX_DEBUG);
	// host cr4 - msr_get(IA32_VMX_CR4_FIXED1)
	vmwrite(VMX_HOST_CR4_N, cr4_get(), VMX_DEBUG);
	

	// host tr
	{
		mprint("tr selector: %x", tr_get());
		vmwrite(VMX_HOST_TR_W, tr_get(), VMX_DEBUG);
	}

	// cr3 target count and other counts
	{
		vmwrite(VMX_GUEST_CR3_T_COUNT_D, 0, VMX_DEBUG);
		vmwrite(VMX_EXIT_MSR_STORE_COUNT_D,0, VMX_DEBUG);
		vmwrite(VMX_EXIT_MSR_LOAD_COUNT_D,0, VMX_DEBUG);
		vmwrite(VMX_ENTRY_MSR_LOAD_COUNT_D,0, VMX_DEBUG);
	}

	{ //t cr3
		vmwrite(VMX_HOST_CR3_N, cr3_get(), VMX_DEBUG);
	}

	{ // host bases
		vmwrite(VMX_HOST_FS_BASE_N, 0, VMX_DEBUG); // flat
		vmwrite(VMX_HOST_GS_BASE_N, 0, VMX_DEBUG); // flat
		vmwrite(VMX_HOST_TR_BASE_N, (uint64_t)0x3000, VMX_DEBUG);
		vmwrite(VMX_HOST_GDTR_BASE_N, getGDTP()->off, VMX_DEBUG);
		vmwrite(VMX_HOST_IDTR_BASE_N, idtr.base, VMX_DEBUG); // todo: this is what a better approach to desc. tables looks like
	}

	{
		vmwrite(VMX_HOST_IA32_SYSTENTER_ESP_N, msr_get(IA32_SYSENTER_ESP), VMX_DEBUG);
		vmwrite(VMX_HOST_IA32_SYSTENTER_EIP_N, msr_get(IA32_SYSENTER_EIP), VMX_DEBUG);
	}

	{
		vmwrite(VMX_HOST_RIP_N, &vmx_return, VMX_DEBUG);
		vmwrite(VMX_HOST_RSP_N, exit_stack+1023, VMX_DEBUG);
	}

	vmwrite(VMX_GUEST_RFLAGS_N, 1 << 1, VMX_DEBUG);
	vmwrite(VMX_GUEST_CR3_N, 0, VMX_DEBUG); // just pass by... please

	{
		uint32_t zero = msr_get(IA32_VMX_CR4_FIXED0);
		uint32_t one = msr_get(IA32_VMX_CR4_FIXED1);
		uint64_t flex = ~zero & one;
		uint64_t vmcr4 = 0;
		//vmcr4 &= flex;
		vmcr4 |= zero;
		vmcr4 &= one;
		vmcr4 &= ~(1<<17); // not ia32-e guest => not cr4.pcide
		vmwrite(VMX_GUEST_CR4_N, vmcr4, VMX_DEBUG);
	}

	vmwrite(VMX_GUEST_DR7_N, 0x400, VMX_DEBUG);
	vmwrite(VMX_GUEST_ES_W, 0, VMX_DEBUG);
	vmwrite(VMX_GUEST_CS_W, 0, VMX_DEBUG);
	vmwrite(VMX_GUEST_SS_W, 0, VMX_DEBUG);
	vmwrite(VMX_GUEST_DS_W, 0, VMX_DEBUG);
	vmwrite(VMX_GUEST_FS_W, 0, VMX_DEBUG);
	vmwrite(VMX_GUEST_GS_W, 0, VMX_DEBUG);

	vmwrite(VMX_GUEST_RSP_N, 0x7200, VMX_DEBUG);
	vmwrite(VMX_GUEST_RIP_N, 0x7000, VMX_DEBUG);

	{
		vmwrite(VMX_GUEST_ES_LIMIT_D,0xffff,VMX_DEBUG);
		vmwrite(VMX_GUEST_CS_LIMIT_D,0xffff,VMX_DEBUG);
		vmwrite(VMX_GUEST_SS_LIMIT_D,0xffff,VMX_DEBUG);
		vmwrite(VMX_GUEST_DS_LIMIT_D,0xffff,VMX_DEBUG);
		vmwrite(VMX_GUEST_FS_LIMIT_D,0xffff,VMX_DEBUG);
		vmwrite(VMX_GUEST_GS_LIMIT_D,0xffff,VMX_DEBUG);	
	}
	

	{ // guest bases
		vmwrite(VMX_GUEST_ES_BASE_N, 0, VMX_DEBUG); // flat
		vmwrite(VMX_GUEST_CS_BASE_N, 0, VMX_DEBUG); // flat
		vmwrite(VMX_GUEST_SS_BASE_N, 0, VMX_DEBUG); // flat
		vmwrite(VMX_GUEST_DS_BASE_N, 0, VMX_DEBUG); // flat
		vmwrite(VMX_GUEST_FS_BASE_N, 0, VMX_DEBUG); // flat
		vmwrite(VMX_GUEST_GS_BASE_N, 0, VMX_DEBUG); // flat
		vmwrite(VMX_GUEST_GDTR_BASE_N, 0, VMX_DEBUG);
		vmwrite(VMX_GUEST_IDTR_BASE_N, 0, VMX_DEBUG);
		vmwrite(VMX_GUEST_GDTR_LIMIT_D, 0, VMX_DEBUG);
		vmwrite(VMX_GUEST_IDTR_LIMIT_D, 0, VMX_DEBUG);
	}

	// we are DOOMED - even LDTR is here
	vmwrite(VMX_GUEST_LDTR_W, 0, VMX_DEBUG);
	vmwrite(VMX_GUEST_LDTR_AR_D, (ar_t){.type = 2, .s = 0, .p = 1, .unuse = 1, .g = 0}.raw, VMX_DEBUG); // unusable; see SDM3B: 26.3.1.2

	vmwrite(VMX_GUEST_TR_W, 0, VMX_DEBUG);
	vmwrite(VMX_GUEST_TR_BASE_N, 0, VMX_DEBUG);
	vmwrite(VMX_GUEST_TR_AR_D, (ar_t){.type = 3, .s = 0, .dpl = 0, .p = 1, .g = 0, .unuse = 0}.raw, VMX_DEBUG);
	vmwrite(VMX_GUEST_TR_LIMIT_D, 0xfff, VMX_DEBUG);

	vmwrite(VMX_GUEST_IA32_SYSENTER_ESP_MSR_N, 0, VMX_DEBUG);
	vmwrite(VMX_GUEST_IA32_SYSENTER_EIP_MSR_N, 0, VMX_DEBUG); // todo: don't use it now

	vmwrite(VMX_GUEST_VMCS_LINK_PTR_Q, 0xFFFFFFFFFFFFFFFF, VMX_DEBUG);
	vmwrite(VMX_GUEST_PENDING_DBG_EXCEPTIONS_N, 0, VMX_DEBUG);
	vmwrite(VMX_GUEST_ACTIVITY_STATE_D, 0, VMX_DEBUG);
	vmwrite(VMX_GUEST_INTERRUPTIBILITY_STATE, 0, VMX_DEBUG);

	//vmwrite(VMX_PREEMPTION_TIMER_VALUE_D, 0xf, VMX_DEBUG);
	eptp_t e = ept_make();
	mprint("%llx", e.raw);
	vmwrite(VMX_EPTP_Q, e.raw, VMX_DEBUG);
	ept_invept(e, 0x1);
	//uint16_t tmp = lar(es_get());
	//mprint("CS: %04x; es ar: %x", cs_get(), lar(es_get()));
	//return 0;
	mprint("");
	asm("cli");
	asm("xchg %bx, %bx");
	vmlaunch(1);
}

int vmx_vmwrite(uint64_t vmcs_id, uint64_t value)
{
	char err;
	asm("vmwrite %2, %1; setna %0"
		: "=r"(err)
		: "r"(vmcs_id), "r"(value));
	return err ? -1 : 0;
}

int vmwrite(uint64_t vmcs_id, uint64_t value, char debug)
{
	if(!debug) return vmx_vmwrite(vmcs_id, value);
	int vmwrite_res = vmx_vmwrite(vmcs_id, value);
	if(!vmwrite_res)
	{
		mprint("Y(%x) ", vmcs_id);
		return 0;
	}
	else
	{
		mprint("N(%x) ", vmcs_id);
		return vmwrite_res;
	}
}

int vmlaunch(char debug)
{
	mprint("1");
	if(!debug) return vmx_vmlaunch();
	int vmlaunch_res = vmx_vmlaunch();
	mprint("2");
	if(!vmlaunch_res)
	{
		return 0;
	}
	else
	{
		//mprint("vmlaunch failed: %s", virt_reason());
		char* rsn = virt_reason();
		mprint("vmlaunch failed: %s", rsn);
		return vmlaunch_res;
	}
}

int vmresume(char debug)
{
	if(!debug) return vmx_vmresume();
	int vmresume_res = vmx_vmresume();
	if(!vmresume_res)
	{
		return 0;
	}
	else
	{
		//mprint("vmlaunch failed: %s", virt_reason());
		char* rsn = virt_reason();
		mprint("vmresume failed: %s", rsn);
		return vmresume_res;
	}
}

int vmx_vmlaunch()
{
	asm("xchg %bx, %bx");
	char err;
	asm("vmlaunch; setna %0"
		: "=r"(err));
	return err ? -1 : 0;
}


int vmx_vmresume()
{
	asm("xchg %bx, %bx");
	char err;
	asm("vmresume; setna %0"
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


extern struct guest_regs gr;

void virt_exit()
{
	asm("cli");
	
	static const char* vmexit_reasons[] = 
	{	"Exception or non-maskable interrupt (NMI)",
		"External interrupt",
		"Triple fault",
		"INIT signal",
		"Start-up IPI (SIPI)",
		"I/O system-management interrupt (SMI)",
		"Other SMI",
		"Interrupt window",
		"NMI window",
		"Task switch",
		"CPUID",
		"GETSEC",
		"HLT",
		"INVD",
		"INVLPG",
		"RDPMC",
		"RDTSC",
		"RSM",
		"VMCALL",
		"VMCLEAR",
		"VMLAUNCH",
		"VMPTRLD",
		"VMPTRST",
		"VMREAD",
		"VMRESUME",
		"VMWRITE",
		"VMXOFF",
		"VMXON",
		"Control-register accesses",
		"MOV DR",
		"I/O instruction",
		"RDMSR",
		"WRMSR",
		"VM-entry failure due to invalid guest state",
		"VM-entry failure due to MSR loading",
		"Reserved",
		"MWAIT",
		"Monitor trap flag",
		"Reserved",
		"MONITOR",
		"PAUSE",
		"VM-entry failure due to machine-check event",
		"Reserved",
		"TPR below threshold",
		"APIC access",
		"Virtualized EOI",
		"Access to GDTR or IDTR",
		"Access to LDTR or TR",
		"EPT violation",
		"EPT misconfiguration",
		"INVEPT",
		"RDTSCP",
		"VMX-preemption timer expired",
		"INVVPID",
		"WBINVD",
		"XSETBV",
		"APIC write",
		"RDRAND",
		"INVPCID",
		"VMFUNC",
		"ENCLS",
		"RDSEED",
		"XSAVES",
		"XRSTORS"};
	int res = vmx_vmread(VMX_EXIT_REASON_D) & 0xFFFF;
	// mprint("VMexit is (0x%x): %s", res, vmexit_reasons[res]);
	// return;
	if(res == 18) // vmcall
	{
		int ax = gr.rax & 0xffff;
		int si = gr.rsi & 0xffff;
		int cx = gr.rcx & 0xffff;
		int bx = gr.rbx & 0xffff;
		//mprint("%d %d %d %d", ax, si, cx, bx);
		if(ax == 42)
		{
			char* s = si;
			printf("\e[%d;1m", bx+30);
			for(int i = 0; i<cx; i++)
				printf("%c", *s++);
			printf("\n");
		}
		else mprint("bad request");
		vmwrite(VMX_GUEST_RIP_N, vmx_vmread(VMX_GUEST_RIP_N)+3, 0);
	}
	else if(res == 0x34)
	{
		mprint("VMexit (0x%x): %s", res, vmexit_reasons[res]);
		vmwrite(VMX_PREEMPTION_TIMER_VALUE_D, 0, VMX_DEBUG);
	}
	else
	{
		mprint("VMexit (0x%x): %s", res, vmexit_reasons[res]);
		for(;;);
	}
	
	return;
}