/**
 * @file paging.c
 * @brief Paging handler.
 */

#include <kernel/paging.h>
#include <kernel/stack.h>
#include <stddef.h>
#include <kernel/vga.h>
#include <kernel/memory.h>

/**
 * @brief      Enables the paging.
 *
 * @param      src   The source of page directory.
 */
extern void enable_paging(void* src);

static void* PD;

#define PAGE_SIZE 0x1000 // = 4K
#define PD_LOW_LIMIT 0x100000 // = 1 MB (it is just better to do so)


/**
 * @brief      Gets the available memory from memory map.
 * @todo Not used yet properly.
 */
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

/**
 * @brief      Initialise page directory
 *
 * @return     PD address.
 */
void* init_PD()
{
	mmap_entry region;
	/*
	for(int i = 1; i < stack_size(); i++) // not the first region - generally used by bootloader, GDT, IDT - no need for mess in there
	{
		region = *((mmap_entry *)stack_get(i));
		if(region.base_low + region.length_low < PD_LOW_LIMIT) continue; // just below limit
		if(region.length_low > 0x1000+0x1000*1024)
		{
			PD = region.base_low; // I hope VERY much, that it takes place in low 32 bits...
			break; // now, region
		}
	}
	*/
	// TODO: fix it (see below)
	// last region is generally the biggest, we'll just use it
	PD = PD_LOW_LIMIT;
	volatile int* e = (volatile int*)PD;
	for(int i = 0; i<1024; i++)
	{
		*e = PD+0x1000*(i+1);
		*e |= pg_P | pg_U;
		*e++;
	}

	return PD;
}

/**
 * @brief      Maps available memory onto memory from beginning. Deprecated and should not be used.
 *
 * @return     Available memory size.
 */
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


/**
 * @brief      Maps one page in page directory.
 *
 * @param[in]  laddr  The logical address of start of the page
 * @param[in]  paddr  The physical address of start of the page.
 * @param[in]  flags  The flags (protection and etc)
 */
void map_page(uint32_t laddr, uint32_t paddr, uint8_t flags)
{
	int *e = (int *)(PD + 0x1000 + (laddr>>12)*4);
	*e = paddr & (~0x111);
	*e |= flags;
}

/**
 * @brief      Gets the physcall address.
 *
 * @param[in]  laddr  The logical address
 *
 * @return     The physcal address.
 * 
 * @todo Not implemented yet!
 */
uint32_t get_paddr(uint32_t laddr)
{
	/*
	int* a = (int*)(PD + 0x1000 + 4*(laddr >> 12)); 
	return (*a & (~0xFFF)) + (laddr & 0xFFF);
	*/
}

/**
 * @brief Turns paging on.
 * Basically an interface on top of enable_paging.
 */
void init_paging()
{
	enable_paging(PD);
}