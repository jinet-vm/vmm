#ifndef VMX_H
#define VMX_H

#include <stdint.h>

// vmx_init.asm todo: get rid of it
extern char vmx_check();

// vmx_enable.c
int vmx_init();
void vmx_crinit();
void vmx_on();

#endif