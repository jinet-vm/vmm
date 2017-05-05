#ifndef VMX_H
#define VMX_H

#include <stdint.h>

// vmx_init.asm
extern char vmx_check();
extern void vmx_crinit();
extern uint64_t vmx_bmsr();

// vmx_enable.c
int vmx_init();

#endif