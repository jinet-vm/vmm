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
	for(i = 3; (i > order) && (*s & 1); i--)
	{
		mprint("%d %llx", i, s);
		s = ((uint64_t)s << 9) | (p[i] << 3);
	}
	mprint("next");
	for(; i > order; i--)
	{
		//mprint("%llx", S & ~0xfffllu);
		//memset(S & ~0xfffllu, 0, 0x1000);
		uint64_t t = physmm_alloc(0);
		mprint("%i %llx %llx", i, s, t);
		uint64_t S = s;
		*s = 1 | t;
		s = (uint64_t)s << 9;
		mprint("clean %llx", s);
		memset((uint64_t)s, 0, 0x1000);
		*s++ = 0xfffffff0;
		*s++ = 0xfffffff0;
		*s++ = 0xfffffff0;
		s--;
		s--;
		s--;
		s += p[i];
	}//ffffff8001000000
	mprint("%i %llx", i, s);
	*s = paddr | 1;
	if(order != 0)
		*s |= PS_BIT;
	//return r;
	pg_invtlb();
}
//000fff8000001800