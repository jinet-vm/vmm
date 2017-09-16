#include <jinet/paging.h>
#include <jinet/module.h>
// hey, guys, it's all recursive mapped somewhere

#define PGDEBUG

MODULE("PAGING");

#define PS_BIT (1 << 7)

uint64_t pg_get_paddr(uint64_t vma)
{
	uint64_t* s = 0xfffffffffffff000;
	uint64_t p[5] = 
	{	vma & 0xfff,
		(vma >> 12) & 0x1ff,
		(vma >> 21) & 0x1ff,
		(vma >> 30) & 0x1ff,
		(vma >> 39) & 0x1ff
	};

	int i;
	s = (uint64_t)s | (p[4] << 3); // first index
	for(i = 3; (i > 0) && (*s & 1) && !(*s & PS_BIT); i--)
		s = ((uint64_t)s << 9) | (p[i] << 3);
	if(!(*s & 1)) // not found
		return 0xffffffffffffffff;

	uint64_t r = *s, mask;
	mask = 1llu << (12+9*i); mask--;
	r &= ~mask;
	r |= vma & mask;

	return r;
}

void pg_invtlb()
{
	asm("mov %%cr3, %%rax\n"
		"mov %%rax, %%cr3"
		: : : "%rax");
}


void pg_map(uint64_t vma, uint64_t paddr, int order)
{
	uint64_t* s = 0xfffffffffffff000;
	uint64_t p[5] = 
	{	vma & 0xfff,
		(vma >> 12) & 0x1ff,
		(vma >> 21) & 0x1ff,
		(vma >> 30) & 0x1ff,
		(vma >> 39) & 0x1ff
	};

	int i;
	s = (uint64_t)s | (p[4] << 3); // first index
	for(i = 3; (i > 0) && (*s & 1); i--)
		s = ((uint64_t)s << 9) | (p[i] << 3);
	for(; i > order; i--)
		*s = 1 | physmm_alloc(0);
	uint64_t r = *s, mask;
	mask = 1llu << (12+9*i); mask--;
	r &= ~mask;
	r |= vma & mask;

	//return r;
}