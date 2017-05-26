#ifndef VMX_H
#define VMX_H

#include <stdint.h>

// vmx_init.asm todo: get rid of itvoid vmwrite(int vmcs_id, int value);
extern char vmx_check();

// vmx_enable.c
int vmx_init();
void vmx_crinit();
void vmx_vmxon();
void vmwrite(uint64_t vmcs_id, uint64_t value);
#endif