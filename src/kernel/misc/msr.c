#include <jinet/msr.h>

uint64_t msr_get(uint32_t msr)
{
	uint32_t low = 0, high = 0;
	asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
	uint64_t val = high;
	val <<= 32;
	val |= low;
	return val;
}

void msr_set(uint32_t msr, uint64_t value)
{
	uint32_t low = value;
	uint32_t high = value >> 32;
	asm volatile("wrmsr" : : "a"(low), "d"(high), "c"(msr));
}