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

#endif