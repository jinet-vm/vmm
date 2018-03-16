#ifndef MSR_H
#define MSR_H

#include <stdint.h>

#define IA32_FEATURE_CONTROL 0x3A
#define IA32_VMX_BASIC 0x480
#define IA32_VMX_PINBASED_CTLS 0x481
#define IA32_VMX_PROCBASED_CTLS 0x482
#define IA32_VMX_EXIT_CTLS 0x483
#define IA32_VMX_ENTRY_CTLS 0x484
#define IA32_VMX_MISC 0x485
#define IA32_VMX_TRUE_PINBASED_CTLS 0x48D
#define IA32_VMX_CR0_FIXED0 0x486
#define IA32_VMX_CR0_FIXED1 0x487
#define IA32_VMX_CR4_FIXED0 0x488
#define IA32_VMX_CR4_FIXED1 0x489
#define IA32_SYSENTER_ESP 0x175
#define IA32_SYSENTER_EIP 0x176
#define IA32_VMX_PROCBASED_CTLS2 0x48B
// if you mess with nonpresent msrs
// you'll f*ck up everything

uint64_t msr_get(uint32_t msr);
void msr_set(uint32_t msr, uint64_t value);

#endif