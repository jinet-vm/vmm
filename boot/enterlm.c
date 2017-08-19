#include <jinet/multiboot2.h>
#include <jinet/bootstruct.h>

void set_pdpt(void *entry, uint64_t addr, int ps);
void set_pd(void* entry, uint64_t addr, int ps);
void set_pt(void* entry, uint64_t addr, int ps);
void set_pf(void* entry, uint64_t addr, int ps);

void* add_table();

uint32_t mem_addr = 0;
void* future_cr3;

void enterlm(void* mb2_info_tags)
{
	// mb2_info_tags: uint32_t total_size, reserved; tags!
	uint32_t frst; frst = mb2_info_tags;
	uint32_t size; size =*(uint32_t*)mb2_info_tags;
	mb2_info_tags += 8;
	uint16_t *t; t = 0xb8000;
	*t++ = 0x4040;
	*t++ = 0x4041;
	*t++ = 0x4042;
	*t++ = 0x4043;
	uint64_t kernel_start = 0, kernel_end = 0;
	uint32_t vd_fb, vd_wd, vd_ht, vd_bpp, vd_type; 
	struct multiboot_mmap_entry* mmap;
	int mmap_num = 0;

	asm("xchg %bx, %bx");

	while(mb2_info_tags < frst+size)
	{
		struct multiboot_tag* tag; tag = mb2_info_tags;
		switch(tag->type)
		{
			case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
			{
				struct multiboot_tag_framebuffer_common* fb; fb = tag;
				vd_fb = (uint32_t)(fb->framebuffer_addr);
				vd_wd = fb->framebuffer_width;
				vd_ht = fb->framebuffer_height;
				vd_bpp = fb->framebuffer_bpp;
				vd_type = fb->framebuffer_type;
				break;
			}
			
			case MULTIBOOT_TAG_TYPE_MODULE:
			{
				struct multiboot_tag_module* mod; mod = tag;
				struct bootstruct* bs; bs = mod->mod_start;
				if(bs->lm_magic != BTSTR_LM_MAGIC) break;
				// we found it!
				kernel_start = mod->mod_start;
				kernel_end = mod->mod_end;
				break;
			}

			case MULTIBOOT_TAG_TYPE_MMAP:
			{
				struct multiboot_tag_mmap* mmapt; mmapt = tag;
				mmap = &mmapt->entries;
				mmap_num = (mmapt->size - 16) / sizeof(struct multiboot_mmap_entry);
				break;
			}

			default :
				break;
		}
		mb2_info_tags += (tag->size + 7) & ~7;
	}
	
	asm("xchg %bx, %bx");

	if(kernel_start == kernel_end || mmap_num == 0)
	{
		int i = 1/0; // and the butchery begins
		// also known as Rebooting Personal Computers 101
	}

	// paging tables

	int i, j; // it's all 'cause it works

	for(i = 0; i<mmap_num; i++)
	{
		if(mmap[i].type == MULTIBOOT_MEMORY_AVAILABLE && mmap[i].len > 0x100000llu) // just in case I need 1024 tables
		{
			if(mmap[i].addr <= kernel_start && kernel_start - mmap[i].addr < mmap[i].len)
			{
				if(kernel_end + 0x100000llu - mmap[i].addr < mmap[i].len) // kernel placed inside that map entry, let's just place memory map & page tables after it
					mem_addr = kernel_end;
			}
			else mem_addr = mmap[i].addr;
		}
	}

	if(!mem_addr) { *(uint32_t*)(1/0) = (uint32_t)1/0;}
	uint64_t mem_addr_orig;
	mem_addr_orig = mem_addr;
	mem_addr = (mem_addr+0x999) & ~0x1000; // page aligned
	struct multiboot_mmap_entry* copy;
	struct multiboot_mmap_entry* copied;
	copy = mem_addr; copied = &mmap[0];
	for(i = 0; i<mmap_num; i++)
	{
		copy->addr = copied->addr;
		copy->len = copied->len;
		copy->type = copied->type;
		copy->zero = 0;
		*copy++; *copied++;
	}
	mem_addr += (mmap_num*sizeof(struct multiboot_mmap_entry)+0x999) & ~0x1000;

	uint64_t kernel_start_pdpt, kernel_start_pd, kernel_start_pt, kernel_start_pf;
	uint64_t kernel_end_pdpt, kernel_end_pd, kernel_end_pt, kernel_end_pf;
	kernel_start_pdpt = (kernel_start >> 39llu) & 0x1ffllu;
	kernel_start_pd = (kernel_start >> 30llu) & 0x1ffllu;
	kernel_start_pt = (kernel_start >> 21llu) & 0x1ffllu;
	kernel_start_pf = (kernel_start >> 12llu) & 0x1ffllu;
	kernel_end_pdpt = (kernel_end >> 39llu) & 0x1ffllu;
	kernel_end_pd = (kernel_end >> 30llu) & 0x1ffllu;
	kernel_end_pt = (kernel_end >> 21llu) & 0x1ffllu;
	kernel_end_pf = (kernel_end >> 12llu) & 0x1ffllu;

	// now mem_addr:	+0000h: PML4 table
	// 					+1000h: PDP table
	// 					+2000h and on: PD and page tables
	future_cr3 = add_table(); // first for PML4
	set_page_entry(mem_addr, mem_addr+0x1000, 0); // kernel PML4
	set_page_entry(mem_addr + 0x1000, mem_addr+0x1000, 0); // kernel PML4
	for(;;);
}

int table_count = 0;

uint64_t map_page_pdpt(uint64_t vma, uint64_t dir_paddr);
uint64_t map_page_dir(uint64_t vma, uint64_t paddr);
uint64_t map_page_tbl(uint64_t vma, uint64_t paddr);
uint64_t map_page_frm(uint64_t vma, uint64_t paddr);

void set_page_entry(void* entry, uint64_t paddr, int ps, int n);

void* add_table()
{
	void* out = mem_addr;
	mem_addr += 0x1000;
	return out;
}


uint64_t map_page_entry(uint64_t vma, uint64_t paddr, int n)
{
	const void* (*sets[4])(void*, uint64_t, int) = {set_pdpt, set_pd, set_pt, set_pf};
	uint64_t* tbl = future_cr3;
	int final = (n > 3 ? 3 : n);
	for(int i = 0; i<=final; i++)
	{
		tbl += ((vma >> (39llu - 9llu*n)) & 0x1ffllu) << 3;
		if(!(*tbl & 1)) // not found!
		{
			if(i == final)
				sets[i](*tbl, paddr, 1);
			else
				sets[i](*tbl, add_table(), 0);
			mem_addr += 0x1000;
			table_count++;
		}
		tbl = *tbl & ~(0xfffllu);
	}

	return tbl;

	if(0 <= n)
	{
		tbl += ((vma >> 39llu) & 0x1ffllu) << 3;
		if(!(*tbl & 1)) // found!
		{
			set_pdpt(tbl, mem_addr);
			mem_addr += 0x1000;
			table_count++;
		}
		tbl = *tbl & ~(0xfffllu); // todo: beware the XD bit
	}
	if(1 <= n)
	{

	}
}

void set_page_entry(void* entry, uint64_t addr, int ps)
{
	uint64_t res = 1; // present flag
	res |= addr & (~0xfffllu);
	res |= (ps != 0) << 7;
	uint64_t* out = entry;
	*out = res;
}

void set_pdpt(void *entry, uint64_t addr, int ps)
{
	set_page_entry(entry, addr & ~0xfffllu, 0);
}

void set_pd(void* entry, uint64_t addr, int ps)
{
	set_page_entry(entry, addr & ~(ps ? 0xffffllu : 0x3fffffffllu), ps);
}

void set_pt(void* entry, uint64_t addr, int ps)
{
	set_page_entry(entry, addr & ~(ps ? 0xffffllu : 0x1fffffllu), ps);
}

void set_pf(void* entry, uint64_t addr, int ps)
{
	set_page_entry(entry, addr & ~0xfffllu, 0);
}