/**
 * @file paging.c
 * @brief Paging handler.
 */

#include <kernel/paging.h>
#include <kernel/stack.h>
#include <stddef.h>
#include <kernel/vga.h>
#include <kernel/memory.h>
#include <kernel/consts.h>

/**
 * @brief      Enables the paging.
 *
 * @param      src   The source of page directory.
 */
extern void enable_paging(void* src);

static void* PD;

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
	PD = PAGING_PHYS_ADDR;
	return PD;
}

/**
 * @brief      Maps available memory onto memory from beginning. Deprecated and should not be used.
 *
 * @return     Available memory size.
 */
size_t map_available_memory() // takes from our stack
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
	*e = paddr & (~0xfff);
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
	// first get PT id
	uint32_t PT_id = laddr >> 22;
	uint32_t PT_off = ((uint32_t *)PD)[PT_id];
	if(!(PT_off & 1)) // not present
		return 0xffffffff;
	PT_off &= ~0xfff;
	uint32_t* PT = (int*)(PT_off);
	 // getting middle 10 bits - ID of page in page table
	uint32_t page_id = (laddr >> 12) & 0x3ff;
	printf("page_id = %x\n",page_id);
	uint32_t page = PT[page_id];
	if(!(page & 1)) // not present
		return 0xffffffff;
	page &= ~0xfff;
	printf("page = %x\n",page);
	return page + (laddr & 0xfff);
}

/**
 * @brief Turns paging on.
 * Basically an interface on top of enable_paging.
 */
void init_paging()
{
	enable_paging(PD);
}