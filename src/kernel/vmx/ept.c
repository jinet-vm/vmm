#include <jinet/heap.h>
#include <jinet/ept.h>
#include <jinet/vmaddr.h>
#include <jinet/paging.h>
#include <jinet/physmm.h>
#include <stdint.h>

ept_t *vbuf = VMX_EPT_VBUF;

static vbuf_turn(uint64_t phys)
{
	// todo: is it effective? writing directly to pt is *better*?
	// todo: use caching?
	pg_map(vbuf, phys, 0);
}

void ept_init()
{
	pg_map(VMX_EPT_VBUF, physmm_alloc(0), 0);
}

void ept_map(ept_t e, uint64_t phys, uint64_t guphy, int order)
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
		//mprint("%d %llx", i, s);
		s = ((uint64_t)s << 9) | (p[i] << 3);
	}
	//mprint("next");
	for(; i > order; i--)
	{
		//mprint("%llx", S & ~0xfffllu);
		//memset(S & ~0xfffllu, 0, 0x1000);
		uint64_t t = physmm_alloc(0);
		//mprint("%i %llx %llx", i, s, t);
		uint64_t S = s;
		*s = 1 | t;
		s = (uint64_t)s << 9;
		s += p[i];
	}//ffffff8001000000
	//mprint("%i %llx", i, s);
	*s = paddr | 1;
	if(order != 0)
		*s |= PS_BIT;
	//return r;
	pg_invtlb();
}

ept_t ept_make()
{
	ept_t e;
	e.raw = physmm_alloc(0); 
	e.ept_ps_mt = EPT_MT_UC;
	e.ad = 0;
}