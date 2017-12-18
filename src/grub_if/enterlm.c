#include <jinet/multiboot2.h>
#include <jinet/bootstruct.h>

uint32_t add_table();
void map_page_entry(uint32_t vma_low, uint32_t vma_high, uint32_t paddr_low, uint32_t paddr_high, int n);

uint32_t mem_addr = 0;
uint64_t future_cr3;
uint64_t ent;

extern void trump();

void enterlm(void* mb2_info_tags)
{
	// mb2_info_tags: uint32_t total_size, reserved; tags!
	uint32_t frst; frst = mb2_info_tags;
	uint32_t size; size = *(uint32_t*)mb2_info_tags;
	mb2_info_tags += 8;
	uint16_t *t; t = 0xb8000;
	*t++ = 0xc040;
	*t++ = 0xd041;
	*t++ = 0xe042;
	*t++ = 0xf043;
	uint32_t kernel_start = 0, kernel_end = 0;
	uint64_t vma_kernel_start;
	uint32_t vd_fb, vd_wd, vd_ht, vd_bpp, vd_type; 
	struct multiboot_mmap_entry* mmap;
	int mmap_num = 0;

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
				vma_kernel_start = bs->lm_load_addr;
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

	if(kernel_start == kernel_end || mmap_num == 0)
	{
		int i = 1/0; // and the butchery begins
		// also known as Rebooting Personal Computers 101
	}

	// paging tables

	int i, j; // it's all 'cause it works

	mem_addr = 0;
	for(i = 0; i<mmap_num; i++)
	{
		// assuming module is in 32 bit address space
		uint64_t len = (mmap[i].len_high*1llu) << 32llu | mmap[i].len_low;
		if(mmap[i].type == MULTIBOOT_MEMORY_AVAILABLE && len > 0x112000llu) // just in case I need 1024 tables + 64kb stack
		{
			if(mmap[i].addr_low <= kernel_start && kernel_start - mmap[i].addr_low < len)
			{
				if(((kernel_end + 0xfff) & ~0xffff) + 0x100000llu - mmap[i].addr_low < len) // kernel placed inside that map entry, let's just place memory map & page tables after it
					mem_addr = ((kernel_end + 0xfff) & ~0xfff);
			}
			else mem_addr = (mmap[i].addr_low + 0xfff) & ~0xfff;
		}
	}	

	if(!mem_addr) { *(uint32_t*)(1/0) = (uint32_t)1/0;}
	uint64_t mem_addr_orig;
	mem_addr_orig = mem_addr;
	mem_addr = (mem_addr+0xfff) & ~0xfff; // page aligned
	struct multiboot_mmap_entry* copy;
	struct multiboot_mmap_entry* copied;
	copy = mem_addr; copied = &mmap[0];
	mmap = mem_addr;
	for(i = 0; i<mmap_num; i++)
	{
		copy->addr_low = copied->addr_low;
		copy->len_low = copied->len_low;
		copy->addr_high = copied->addr_high;
		copy->len_high = copied->len_high;
		copy->type = copied->type;
		copy->zero = 0;
		*copy++; *copied++;
	}


	mem_addr += mmap_num*sizeof(struct multiboot_mmap_entry)+0xfff;
	mem_addr &= ~0xfff;
	// now mem_addr:	+0000h: PML4 table
	// 					+1000h: PDP table
	// 					+2000h and on: PD and page tables
	future_cr3 = add_table();
	unsigned int vma_low, vma_high;
	unsigned int* r = &vma_kernel_start;
	*r++;
	vma_low = vma_kernel_start & 0xffffffff;
	vma_high = *r;
	for(unsigned int lowk = kernel_start; lowk < kernel_end; lowk+=0x1000, vma_low += 0x1000)
		map_page_entry(vma_low, 0xffff8000, lowk, 0, 3); // page-style paging - maybe it's inefficient, but who cares?

	//map_page_entry(0x00000000, 0x1000, 0, 0, 3);
	uint32_t DJT = &trump;
	map_page_entry((DJT & ~0xfff)+0x0000, 0, (DJT & ~0xfff)+0x0000, 0, 3);
	map_page_entry((DJT & ~0xfff)+0x1000, 0, (DJT & ~0xfff)+0x1000, 0, 3);
	map_page_entry((DJT & ~0xfff)+0x2000, 0, (DJT & ~0xfff)+0x2000, 0, 3);

	// recursive mapping
	void* _t = future_cr3+0xff8;
	set_page_entry(_t, future_cr3, 0, 0);

	// stack -- todo: it's hardcoded - baaad
	mem_addr_orig += 0x112000llu;
	for(unsigned int i = 0; i<0x10000; i+=0x1000)
		map_page_entry(0x0000f000-i, 0xffff8000, mem_addr_orig+0x111000-i, 0, 3);

	struct bootstruct* bs = kernel_start;

	// mmap
	uint64_t vmmap = bs->lm_mmap_addr;
	uint32_t vmmap_low = vmmap;
	uint32_t vmmap_high = vmmap >> 32llu;
	map_page_entry(vmmap_low, vmmap_high, mmap, 0, 3);

	// bootstruct
	bs->tr_magic = BTSTR_TR_MAGIC;
	bs->tr_video_type = vd_type;
	bs->tr_vd_framebuffer = vd_fb;
	bs->tr_vd_width = vd_wd;
	bs->tr_vd_height = vd_ht;
	bs->tr_vd_depth = vd_bpp;
	bs->tr_mmap_len = mmap_num;
	bs->tr_pgtb_start = mem_addr_orig;
	bs->tr_pgtb_cur = mem_addr;
	ent = bs->lm_entry_addr;
}

int table_count = 0;

uint32_t add_table()
{
	uint32_t out = mem_addr;
	uint32_t *r = mem_addr;
	for(int i = 0; i<1024; i++)
	{
		*r = 0x0;
		*r++;
	}
	mem_addr += 0x1000;
	return out;
}


void map_page_entry(uint32_t vma_low, uint32_t vma_high, uint32_t paddr_low, uint32_t paddr_high, int n)
{
	void* tbl = future_cr3;
	int final = (n > 3 ? 3 : n);
	int nums[4] = {(vma_high >> 7) & 0x1ff, ((vma_high & 0x3f) << 2) | (vma_low & (3 << 30)), (vma_low >> 21) & 0x1ff, (vma_low >> 12) & 0x1ff};
	for(int i = 0; i<=final; i++)
	{
		// vma_high = vma >> 32;
		// vma_low = vma & 0xffffffff
		// vma >> 39 & 0x1ff = vma_high >> 7 & 0x1ff
		// vma >> 30 & 0x1ff = ((vma_high & 0x3f) << 2) | (vma_low & (3 << 30))
		// vma >> 21 & 0x1ff = vma_low >> 21 & 0x1ff
		tbl += nums[i]*8;
		if(!(*(int *)tbl & 1)) // not found!
		{
			if(i == final)
				set_page_entry(tbl, paddr_low, paddr_high, i != 3);
			else
				set_page_entry(tbl, add_table(), 0, 0);
			table_count++;
		}
		tbl = *(unsigned int *)tbl & ~(0xfffllu); // 32 bit space
	}
}

void set_page_entry(void* entry, uint32_t addr_low, uint32_t addr_high, int ps)
{
	// //asm("xchg %bx, %bx");
	// //asm("xchg %bx, %bx");
	uint32_t* low = entry, *high = entry + 4;
	*low = 1;
	*low |= (ps != 0) << 7;
	*low |= addr_low & (~0xfff);
	*high |= addr_high;
}