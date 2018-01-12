#ifndef VMX_H
#define VMX_H

#include <stdint.h>

// >> functions

// prefixes:
// int vmx_<vmx instruction>(T values) - interface to intel vmx (probably has zf & cf flag check)
// T virt_<...> - jinet function
// int <vmx instruction>(T values, char debug) - wrap-around of vmx_<vmx instruction> with error handling (vmx reasons); if debug => output

// vmx_init.asm todo: get rid of it
extern char vmx_check();

// vmx_enable.c
int virt_init();
char* virt_reason();
void virt_crinit();
int vmx_vmxon();
int vmx_vmwrite(uint64_t vmcs_id, uint64_t value);
int vmx_vmwrite16(uint64_t vmcs_id, uint16_t value);
uint64_t vmx_vmread(uint64_t vmcs_id);
int vmwrite(uint64_t vmcs_id, uint64_t value, char debug);
int vmx_vmlaunch();
int vmx_vmresume();
int vmlaunch(char debug);
int vmresume(char debug);
void virt_exit();

// >> vmcs fields

// size suffixes:
// W - word (16 bit)
// D - dword (32 bit)
// Q - qword (64 bit)
// N - natural width

// todo: enum this thing up!

#define VMX_PINBASED_CTLS_D 0x4000
#define VMX_PROCBASED_CTLS_D 0x4002
#define VMX_SEC_PROCBASED_CTLS_D 0x401E
#define VMX_VMEXIT_CTLS_D 0x400C
#define VMX_VMENTRY_CTLS_D 0x4012

#define VMX_GUEST_GDTR_LIMIT_D 0x4810
#define VMX_GUEST_IDTR_LIMIT_D 0x4812

#define VMX_HOST_CR0_N 0x6C00
#define VMX_HOST_CR3_N 0x6C02
#define VMX_HOST_CR4_N 0x6C04
#define VMX_HOST_FS_BASE_N 0x6C06
#define VMX_HOST_GS_BASE_N 0x6C08
#define VMX_HOST_TR_BASE_N 0x6C0A
#define VMX_HOST_GDTR_BASE_N 0x6C0C
#define VMX_HOST_IDTR_BASE_N 0x6C0E
#define VMX_HOST_IA32_SYSTENTER_ESP_N 0x6C10
#define VMX_HOST_IA32_SYSTENTER_EIP_N 0x6C12
#define VMX_HOST_RSP_N 0x6C14
#define VMX_HOST_RIP_N 0x6C16

#define VMX_HOST_ES_W 0xC00
#define VMX_HOST_CS_W 0xC02
#define VMX_HOST_SS_W 0xC04
#define VMX_HOST_DS_W 0xC06
#define VMX_HOST_FS_W 0xC08
#define VMX_HOST_GS_W 0xC0A
#define VMX_HOST_TR_W 0xC0C

#define VMX_HOST_FS_BASE_N 0x6C06
#define VMX_HOST_GS_BASE_N 0x6C08
#define VMX_HOST_TR_BASE_N 0x6C0A
#define VMX_HOST_GDTR_BASE_N 0x6C0C
#define VMX_HOST_IDTR_BASE_N 0x6C0E

#define VMX_GUEST_CR3_T_COUNT_D 0x400A
#define VMX_EXIT_MSR_STORE_COUNT_D 0x400E
#define VMX_EXIT_MSR_LOAD_COUNT_D 0x4010
#define VMX_ENTRY_MSR_LOAD_COUNT_D 0x4014

#define VMX_GUEST_CR0_N 0x6800
#define VMX_GUEST_CR3_N 0x6802
#define VMX_GUEST_CR4_N 0x6804
#define VMX_GUEST_DR7_N 0x681A
#define VMX_GUEST_ES_BASE_N 0x6806
#define VMX_GUEST_CS_BASE_N 0x6808
#define VMX_GUEST_SS_BASE_N 0x680A
#define VMX_GUEST_DS_BASE_N 0x680C
#define VMX_GUEST_FS_BASE_N 0x680E
#define VMX_GUEST_GS_BASE_N 0x6810
#define VMX_GUEST_TR_BASE_N 0x6814
#define VMX_GUEST_GDTR_BASE_N 0x6816
#define VMX_GUEST_IDTR_BASE_N 0x6818
#define VMX_GUEST_RFLAGS_N 0x6820

#define VMX_GUEST_RSP_N 0x681C
#define VMX_GUEST_RIP_N 0x681E

#define VMX_GUEST_LDTR_W 0x80C

#define VMX_GUEST_ES_W 0x800
#define VMX_GUEST_CS_W 0x802
#define VMX_GUEST_SS_W 0x804
#define VMX_GUEST_DS_W 0x806
#define VMX_GUEST_FS_W 0x808
#define VMX_GUEST_GS_W 0x80A
#define VMX_GUEST_TR_W 0x80E

#define VMX_GUEST_ES_AR_D 0x4814 // todo: WHY?! the ar is a _byte_
#define VMX_GUEST_CS_AR_D 0x4816
#define VMX_GUEST_SS_AR_D 0x4818
#define VMX_GUEST_DS_AR_D 0x481A
#define VMX_GUEST_FS_AR_D 0x481C
#define VMX_GUEST_GS_AR_D 0x481E
#define VMX_GUEST_TR_AR_D 0x4822

#define VMX_GUEST_ES_LIMIT_D 0x00004800
#define VMX_GUEST_CS_LIMIT_D 0x00004802
#define VMX_GUEST_SS_LIMIT_D 0x00004804
#define VMX_GUEST_DS_LIMIT_D 0x00004806
#define VMX_GUEST_FS_LIMIT_D 0x00004808
#define VMX_GUEST_GS_LIMIT_D 0x0000480A
#define VMX_GUEST_TR_LIMIT_D 0x0000480E

#define VMX_GUEST_LDTR_AR_D 0x4820

#define VMX_GUEST_IA32_SYSENTER_ESP_MSR_N 0x6824
#define VMX_GUEST_IA32_SYSENTER_EIP_MSR_N 0x6826

#define VMX_GUEST_VMCS_LINK_PTR_Q 0x2800

#define VMX_GUEST_PENDING_DBG_EXCEPTIONS_N 0x6822

#define VMX_GUEST_ACTIVITY_STATE_D 0x4826
#define VMX_GUEST_INTERRUPTIBILITY_STATE 0x4824

#define VMX_EXIT_REASON_D 0x4402

#define VMX_PREEMPTION_TIMER_VALUE_D 0x482E

struct guest_regs
{
	uint64_t rax, rsi, rcx, rbx; // the only things i need
} __attribute__((packed));

#endif