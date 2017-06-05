#ifndef VMX_H
#define VMX_H

#include <stdint.h>

// >> functions

// prefixes:
// vmx_<vmx instruction> - interface to intel vmx (probably has zf & cf flag check)
// virt_<...> - jinet function
// <vmx instruction> - wrap-around of vmx_<vmx instruction> with error handling (vmx reasons)

// vmx_init.asm todo: get rid of it
extern char vmx_check();

// vmx_enable.c
int virt_init();
void virt_crinit();
int vmx_vmxon();
int vmx_vmwrite(uint64_t vmcs_id, uint64_t value);
int vmx_vmwrite16(uint64_t vmcs_id, uint16_t value);
uint64_t vmx_vmread(uint64_t vmcs_id);

// >> vmcs fields

// size suffixes:
// W - word (16 bit)
// D - dword (32 bit)
// Q - qword (64 bit)
// N - natural width

#define VMX_PINBASED_CTLS_D 0x4000
#define VMX_PROCBASED_CTLS_D 0x4002
#define VMX_SEC_PROCBASED_CTLS_D 0x401E
#define VMX_VMEXIT_CTLS_D 0x400C
#define VMX_VMENTRY_CTLS_D 0x4012

#define VMX_HOST_CR0_N 0x6C00
#define VMX_HOST_CR3_N 0x6C02
#define VMX_HOST_CR4_N 0x6C04
#define VMX_HOST_FS_N 0x6C06
#define VMX_HOST_GS_N 0x6C08
#define VMX_HOST_TR_N 0x6C0A
#define VMX_HOST_GDTR_N 0x6C0C
#define VMX_HOST_IDTR_N 0x6C0E
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

#define VMX_GUEST_CR0_N 0x6800

#endif