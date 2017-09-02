#include <jinet/paging.h>
#include <jinet/module.h>
// hey, guys, it's all recursive mapped somewhere

MODULE("PAGING");

uint64_t pg_get_paddr(uint64_t vma)
{;
	mprint("%llx", vma);
	uint64_t p[5] = 
	{	vma & 0xfff,
		(vma >> 12) & 0x1ff,
		(vma >> 21) & 0x1ff,
		(vma >> 30) & 0x1ff,
		(vma >> 39) & 0x1ff
	};

	uint64_t np[5] = {0,0,0,0,0};
	int i; uint64_t faddr;

	for(int i = 3; i >= 0; i--)
	{
		for(int j = 0; j < 5; j++)
			if(i+j >= 5)
				np[j] = 0xfff;
			else
				np[j] = p[i+j];
		uint64_t a = 0;
		np[0] <<= 3;
		a |= np[0] & 0xfff;
		for(int j = 1; j<5; j++)
			a |= (np[j] & 0x1ffllu) << (12+9*(j-1));
		if(a & (1llu << 47))
		{
			//mprint("g");
			a |= 0xffff000000000000llu;
		}
		uint64_t *p = a;
		if(!(*p & 1)) // page not present
			return 0;
		if(i == 1 || (*p && (1 << 7))) // not a table
		{
			faddr = a;
			faddr &= ~0xfff;
			faddr &= ~(1llu << 63); // XD bit
			faddr |= p[0];
			for(int j = 0; j < i; j++)
				faddr |= (p[j] & 0x1ffllu) << (12+9*(j-1));
		}
		mprint("%llx: %x %x %x %x %x", a, np[4], np[3], np[2], np[1], np[0]);
		mprint(" => 0x%016llx", *p);
	}

	// for(i = 0; i >= 0; i--)
	// {
	// 	//uint64_t p_a = 0xfff000000000000llu | (0x1ffllu << 39); //~((~0llu) >> (16 + 12 + 9*(i+1)));
	// 	uint64_t p_a = 0xfff000000000000llu;
	// 	for(int j = 0; j )
	// 	p_a |= (vma >> (12+9*i));
	// 	mprint("%llx", p_a);
	// 	uint64_t p = *(uint64_t*)p_a;
	// 	if(i == 0 || (p & (1 << 7))) // page size
	// 	{
	// 		faddr = p & ~((1 << (13 + 9*i)) - 1);
	// 		faddr += vma & ((1 << (13 + 9*i)) - 1);
	// 		break;
	// 	}
	// }

	return faddr;
}