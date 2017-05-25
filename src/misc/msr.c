#include <kernel/msr.h>

void msr_get(uint32_t msr, uint32_t *low, uint32_t *high)
{
	asm volatile("rdmsr" : "=a"(*low), "=d"(*high) : "c"(msr));
}

void msr_set(uint32_t msr, uint32_t low, uint32_t high)
{
	asm volatile("wrmsr" : : "a"(low), "d"(high), "c"(msr));
}