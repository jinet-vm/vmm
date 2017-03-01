#include <paging.h>
#include <stack.h>
#include <stddef.h>
#include <vga.h>
#include <memory.h>

extern void hex_f(int n, char *s);
extern void enable_paging(void* dst);

static void* PD;

void get_available_memory()
{
	volatile mmap_entry* region = (volatile mmap_entry*)(0xF000);	
	while(region->type != 0)
	{
		if(region->type == 1)
			stack_push(region);
		*region++;
	}
}

void* init_PD()
{
	mmap_entry region;
	for(int i = 1; i < stack_size(); i++) // not the first region - generally used by bootloader, GDT, IDT - no need for mess in there
	{
		region = *((mmap_entry *)stack_get(i));
		if(region.length_low > 0x1000+0x1000*1024)
		{
			PD = region.base_low; // I hope VERY much, that it takes place in low 32 bits...
			break; // now, region
		}
	}

	volatile int* e = (volatile int*)PD;
	for(int i = 0; i<1024; i++)
	{
		*e = PD+0x1000*(i+1);
		*e |= pg_P | pg_U;
		*e++;
	}

	return PD;
}

size_t map_available_memory() // takes from stack
{
	size_t memsize = 0;

	memset(PD+0x1000,0,0x1000*1024);

	volatile int* e = (volatile int*)(PD+0x1000);

	int R = 0;
	mmap_entry region = *((mmap_entry *)stack_get(R));
	for(int i = 0; i<1024*1024; i++)
	{
		*e = region.base_low;
		*e |= pg_P | pg_U | pg_R;

		if(region.length_low < 0x1000)
		{
			*e = 0;
			continue;
		}

		region.length_low-=0x1000;
		memsize += 0x1000;
		region.base_low+=0x1000;

		if((region.length_low < 0x1000) && R < stack_size())
		{
			R++;
			region = *((mmap_entry *)stack_get(R));
		}

		*e++;
	}
	return memsize;
}

void map_page(uint32_t laddr, uint32_t paddr, uint8_t flags)
{
	int *e = (int *)(PD + 0x1000 + (laddr>>12)*4);
	*e = paddr & (~0x111);
	*e |= flags;
}


// TODO: logical -> physical
uint32_t get_paddr(uint32_t laddr)
{
	/*
	int* a = (int*)(PD + 0x1000 + 4*(laddr >> 12)); 
	return (*a & (~0xFFF)) + (laddr & 0xFFF);
	*/
}

void init_paging()
{
	enable_paging(PD);
}