#include <jinet/heap.h>
#include <jinet/ept.h>
#include <jinet/vmaddr.h>
#include <jinet/paging.h>
#include <jinet/physmm.h>
#include <jinet/module.h>
#include <stdint.h>

// todo: omg, how unsecure it is in SMP; multiple vbufs for each core?
ept_entry_t *vbuf = VMA_EPT_VBUF;

static vbuf_turn(uint64_t phys)
{
	// todo: is it effective? writing directly to pt is *better*?
	// todo: use caching?
	pg_map(vbuf, phys, 0);
}

void ept_init()
{
	pg_map(VMA_EPT_VBUF, physmm_alloc(0), 0);
}

void ept_map(eptp_t e, uint64_t phys, uint64_t guphy, int order)
{
	// vbuf_turn(e.raw & ~0xfff);
	// uint64_t p[5] = 
	// {	guphy & 0xfff,
	// 	(guphy >> 12) & 0x1ff,
	// 	(guphy >> 21) & 0x1ff,
	// 	(guphy >> 30) & 0x1ff,
	// 	(guphy >> 39) & 0x1ff
	// };
	// int i;
	// s = (uint64_t)s | (p[4] << 3); // first index
	// for(i = 3; (i > order) && (*s & 1); i--)
	// {
	// 	s = ((uint64_t)s << 9) | (p[i] << 3);
	// }
	// for(; i > order; i--)
	// {
	// 	uint64_t t = physmm_alloc(0);
	// 	uint64_t S = s;
	// 	*s = 1 | t;
	// 	s = (uint64_t)s << 9;
	// 	s += p[i];
	// }//ffffff8001000000
	// //mprint("%i %llx", i, s);
	// *s = paddr | 1;
	// if(order != 0)
	// 	*s |= PS_BIT;
	// //return r;
	// pg_invtlb();
}

eptp_t ept_make()
{
	eptp_t e;
	e.raw = physmm_alloc(0); 
	e.ept_ps_mt = EPT_MT_UC;
	e.ept_pw_ln = 3;
	e.ad = 1;
	vbuf_turn(e.raw & ~0xfff);
	vbuf[0].raw = physmm_alloc(0);
	vbuf[0].x = vbuf[0].xu = vbuf[0].r = vbuf[0].w = 1;
	vbuf_turn(vbuf[0].raw & ~0xfff);
	vbuf[0].raw = 0;
	vbuf[0].mr = 1; // 1 gib page
	vbuf[0].x = vbuf[0].xu = vbuf[0].r = vbuf[0].w = 1;
	mprint("%016llx", vbuf[0].raw);
	return e;
}

struct invept_desc
{
	uint64_t ept;
	uint64_t zero;
} __attribute__((packed));

extern _invept(uint64_t type, void* dsc);

void ept_invept(eptp_t e, uint64_t type)
{
	struct invept_desc id;
	id.ept = e.raw;
	id.zero = 0;
	uint64_t u = &id;
	// asm("invept 0, $1"
	//  	: : "m"(u));
	_invept(type, &id);
}