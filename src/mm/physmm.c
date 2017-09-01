#include <jinet/physmm.h>
#include <jinet/module.h>

void physmm_init(struct multiboot_mmap_entry* map)
{
	for(int i = 0; i < bs.tr_mmap_len; i++)
	{
		uint64_t start = mmap[i].addr;
		uint64_t end = mmap[i].addr + mmap[i].len;
		mprint("mmap 0x%x%x - 0x%x%x", start >> 32, start, end >> 32, end);
	}
}