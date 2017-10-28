#include <jinet/regs.h>

uint64_t cr0_get()
{
	uint64_t val;
	asm("movq %%cr0, %0"
		: "=r"(val));
	return val;
}

void cr0_set(uint64_t value)
{
	asm("movq %0, %%cr0"
		: : "r"(value));
}

uint64_t cr4_get()
{
	uint64_t val;
	asm("movq %%cr4, %0"
		: "=r"(val));
	return val;
}

void cr4_set(uint64_t value)
{
	asm("movq %0, %%cr4"
		: : "r"(value));
}

uint16_t tr_get()
{
	static uint16_t val = 0;
	asm("str %0"
		: : "m"(val));
	return val;
}

void tr_set(uint16_t val)
{
	asm("ltr %0"
		: : "r"(val));
	//return val;
}

uint64_t cr3_get()
{
	uint64_t val;
	asm("mov %%cr3, %0"
		: "=r"(val));
	return val;
}

uint16_t cs_get()
{
	uint16_t val;
	asm("mov %%cs, %0"
		: "=r"(val));
	return val;
}

uint16_t ds_get()
{
	uint16_t val;
	asm("mov %%ds, %0"
		: "=r"(val));
	return val;
}

void ds_set(uint16_t value)
{
	asm("mov %%ax, %%ds"
		: : "a"(value));
}

uint16_t fs_get()
{
	uint16_t val;
	asm("mov %%fs, %0"
		: "=r"(val));
	return val;
}

void fs_set(uint16_t value)
{
	asm("mov %%ax, %%fs"
		: : "a"(value));
}

uint16_t es_get()
{
	uint16_t val;
	asm("mov %%es, %0"
		: "=r"(val));
	return val;
}

void es_set(uint16_t value)
{
	asm("mov %%ax, %%es"
		: : "a"(value));
}

uint16_t gs_get()
{
	uint16_t val;
	asm("mov %%gs, %0"
		: "=r"(val));
	return val;
}

void gs_set(uint16_t value)
{
	asm("mov %%ax, %%gs"
		: : "a"(value));
}

uint16_t ss_get()
{
	uint16_t val;
	asm("mov %%ss, %0"
		: "=r"(val));
	return val;
}

void ss_set(uint16_t value)
{
	asm("mov %%ax, %%ss"
		: : "a"(value));
}

uint16_t lar(uint16_t seg)
{
	uint16_t ar = 0; // todo: why?? f*ck you gcc
	uint32_t t = (uint32_t)seg;
	asm("lar %1, %0"
		: "=d"(ar)
		: "b"(t));
	return ar >> 8;
}