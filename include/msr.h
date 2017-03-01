#ifndef MSR_H
#define MSR_H

#include <stdint.h>

void msr_get(uint32_t num, uint32_t* low, uint32_t *high);
void msr_set(uint32_t num, uint32_t low, uint32_t high);

#endif