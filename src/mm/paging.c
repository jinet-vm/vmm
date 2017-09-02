#include <jinet/paging.h>
#include <jinet/module.h>
// hey, guys, it's all recursive mapped somewhere

MODULE("PAGING");

uint64_t pg_get_paddr(uint64_t vma)
{
	uint64_t p[5] = 
	{	vma & 0xfff,
		(vma >> 12) & 0x1ff,
		(vma >> 21) & 0x1ff,
		(vma >> 30) & 0x1ff,
		(vma >> 39) & 0x1ff
	};

	uint64_t np[5] = {0,0,0,0,0};
	int i; uint64_t faddr;

	for(int i = 2; i >= 1; i--)
	{
		for(int j = 0; j < 5; j++) // forming a new recursive mapping request
			if(i+j >= 5)
				np[j] = 0xfff;
			else
				np[j] = p[i+j];
		uint64_t a = 0;
		np[0] <<= 3;
		a |= np[0] & 0xfff;
		for(int j = 1; j<5; j++)
			a |= (np[j] & 0x1ffllu) << (12+9*(j-1));
		if(a & (1llu << 47)) // sign bit
			a |= 0xffff000000000000llu;
		uint64_t *page = a;
		if(!(*page & 1)) // page not present
			return 0;
		if(i == 1 || (*page & (1llu << 7))) // not a table - either page table entry or page bit set
		{
			faddr = *page;
			faddr &= ~0xfffllu;
			faddr &= ~(1llu << 63); // XD bit
			faddr |= p[0];
			for(int j = 1; j < i; j++)
				faddr |= (p[j] & 0x1ffllu) << (12+9*(j-1));
			break;
		}
	}
	return faddr;
}