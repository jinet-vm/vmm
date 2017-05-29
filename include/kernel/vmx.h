#ifndef VMX_H
#define VMX_H

#include <stdint.h>

// vmx_init.asm todo: get rid of it
extern char vmx_check();

// vmx_enable.c
int vmx_init();
void vmx_crinit();
int vmx_vmxon();
int vmx_vmwrite(uint64_t vmcs_id, uint64_t value);
uint64_t vmx_vmread(uint64_t vmcs_id);

#endif