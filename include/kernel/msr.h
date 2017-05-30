#ifndef MSR_H
#define MSR_H

#include <stdint.h>

#define IA32_FEATURE_CONTROL 0x3A

void msr_get(uint32_t num, uint32_t* low, uint32_t *high);
void msr_set(uint32_t num, uint32_t low, uint32_t high);

#endif