#include <jinet/physmm.h>
#include <jinet/module.h>
#include <jinet/paging.h>
#include <jinet/bootstruct.h>
#include <jinet/vmaddr.h>


MODULE("PHYSMM");

static uint8_t* bitmap[11];
static uint64_t boffs[11];
static uint64_t _ts;

// basic_alloc is used only ONCE
static int bbd_init(uint64_t total_size, void* (*basic_alloc)(uint64_t))
{
	mprint("bbd_init");
	_ts = total_size;
	uint64_t compmal = 0;
	boffs[0] = 0;
	for(int j = 0; j<10; j++)
	{
		uint64_t page = 0x1000 << j;
		uint64_t size = (total_size + page - 1) / page;
		size = (size + 7) / 8;
		boffs[j+1] = boffs[j] + size;
		compmal += size;
		page *= 2;
	}

	mprint("size: %llx", compmal);
	bitmap[0] = (uint8_t*) basic_alloc(compmal);
	asm("xchg %bx, %bx");
	memset(VMA_BBD_BITMAP, 0, 0x100000);
	for(int i = 1; i < 10; i++)
	{
		bitmap[i] = bitmap[0] + boffs[i];
		mprint("a %llx", bitmap[i]);
	}

	// for(uint8_t* t = bitmap[0]; t < bitmap[0]+compmal; t++)
	// 	*t = 0;
	return 0;
}

static int bbd_free_page(uint64_t addr, int order)
{
	uint8_t* bm = bitmap[order];
	uint8_t bit = addr >> (order + 12);
	bm[bit / 8] |= 1 << (bit % 8);
	return 0;
}

static int bbd_use_page(uint64_t addr, int order)
{
	uint8_t* bm = bitmap[order];
	uint8_t bit = addr >> (order + 12);
	bm[bit / 8] &= ~(1 << (bit % 8));
	return 0;
}

static int bbd_get_page(uint64_t addr, int order)
{
	uint8_t* bm = bitmap[order];
	uint8_t bit = addr >> (order + 12);
	return !!(bm[bit / 8] & 1 << (bit % 8));
}

static int bbd_add_region(uint64_t addr, uint64_t size)
{
	uint64_t p = addr;
	while(p < addr+size)
	{
		mprint("%llx ?", p);
		uint64_t page = 0x1000, porder = 0;
		for(page = 0x1000, porder = 0;
			porder < 9 && (p + (page << 1)) <= (addr+size) && !(p & ((page << 1) - 1));
			page <<= 1, porder++);
		mprint("%i %llx !", porder, p);
		bbd_free_page(p, porder);
		p += page;
	}
	return 0;
}

static int bbd_use_region(uint64_t addr, uint64_t size)
{
	uint64_t p = addr;
	while(p < addr+size)
	{
		uint64_t page = 0x1000, porder = 0;
		for(page = 0x1000, porder = 0;
			(p + (page << 1)) <= (addr+size) && !(p & ((page << 1) - 1));
			page <<= 1, porder++);
		if(porder) page >>= 1, porder--;
		bbd_use_page(p, porder);
		p += page;
	}
}

static int bbd_split_block(uint64_t addr, int order)
{
	if(!bbd_get_page(addr, order) || order == 0) return 1;
	bbd_use_page(addr, order);
	order--;
	bbd_free_page(addr, order);
	bbd_free_page(addr+(0x1000 << order), order);
	return 0;
}

static int bbd_unite_block(uint64_t addr, int order)
{
	uint64_t page = 0x800 << order;
	addr &= ~(page - 1);
	if(order <= 0 || order >= 10 || !bbd_get_page(addr, order-1) || !bbd_get_page(addr + page, order-1)) return 1;
	bbd_use_page(addr, order-1);
	bbd_use_page(addr + page, order-1);
	bbd_free_page(addr, order);
	return 0;
}

static uint64_t bbd_alloc(int order)
{
	// TODO: use a better tree-based approach for EVERY bitmap (like sqrt-decomposition?)
	// simply using binary might turn this from linear to log complexity
	// which is kinda awesome but doubles memory use (see 1+1/2+...1/2^n = 2)
	//mprint("bbd alloc");
	int o = order;
	uint64_t block = 0;
	while(o < 10)
	{

		uint64_t page = 0x1000 << o;
		uint64_t i;
		for(i = 0; i < _ts; i+=page)
			if(bbd_get_page(i, o))
				break;
		//mprint("%llx, %i", i, o);
		if(i != _ts) // found!
		{
			block = i;
			break;
		}
		o++;
	}

	for(; o > order; o--)
		bbd_split_block(block, o);

	bbd_use_page(block, o);
	return block;
}

static int bbd_free(uint64_t addr, int order) // TODO: there must be a way without _order_, huh? in libc you don't have to provide that
{
	// unite next block of the same order
	bbd_free_page(addr, order++);

	while(!bbd_unite_block(addr & ((1llu << order) - 1llu), order++));
	return 0;
}

static struct multiboot_mmap_entry* _mmap;
static int _num;

static void* basic_alloc(uint64_t size);

extern void *KERNEL_START, *KERNEL_END;
extern struct bootstruct bs;


static uint64_t min(uint64_t a, uint64_t b)
{
	return (a < b ? a : b);
}

static uint64_t max(uint64_t a, uint64_t b)
{
	return a + b - min(a,b);
}


void physmm_init(struct multiboot_mmap_entry* mmap, int num)
{
	mprint("pi");
	_mmap = mmap;
	_num = num;
	uint64_t ts = 0;
	for(int i = 0; i < num; i++)
	{
		if(ts < mmap[i].addr + mmap[i].len)
			ts = mmap[i].addr + mmap[i].len;
		if(mmap[i].type == MULTIBOOT_MEMORY_AVAILABLE)
			mprint("%016llx : %llx", mmap[i].addr, mmap[i].len);
	}
	bbd_init(ts, &basic_alloc);
	for(int i = 0; i<num; i++)
	{
		mprint("%llx : %llx", mmap[i].addr, mmap[i].len);
		if(mmap[i].type == MULTIBOOT_MEMORY_AVAILABLE)
			bbd_add_region(mmap[i].addr, mmap[i].len);
	}
	mprint("done?");
	physmm_use(0x4000,0x4000);
	uint64_t pks = min(pg_get_paddr(&KERNEL_START), bs.tr_pgtb_start), pke = bs.tr_pgtb_cur;
	physmm_use(pks, pke-pks);
}

uint64_t physmm_alloc(int order)
{
	return bbd_alloc(order);
}

void physmm_free(uint64_t paddr, int order)
{
	bbd_free(paddr, order);
}

void physmm_use(uint64_t addr, uint64_t size)
{
	bbd_use_region(addr, size);
}

void physmm_add_region(uint64_t addr, uint64_t size)
{
	bbd_add_region(addr, size);
}

// this thing does a lot of things (_mmap and _num required; called only once)
// 1. find physical memory (_size_)
// 2. map it somewhere, initialize it (?) and return


static void *basic_alloc(uint64_t size)
{
	//uint64_t pks = pg_get_paddr(&KERNEL_START), pke = bs.tr_pgtb_start+0x135000;
	uint64_t pks = min(pg_get_paddr(&KERNEL_START), bs.tr_pgtb_start), pke = max(bs.tr_pgtb_start+0x135000, bs.tr_pgtb_cur);
	mprint("%llx %llx", pks, pke);

	uint64_t pgtbs = bs.tr_pgtb_start;

	// 1. physical memory
	uint64_t paddr = 0;
	for(int i = 0; (i<_num) && (!paddr); i++)
	{
		if(_mmap[i].type != MULTIBOOT_MEMORY_AVAILABLE)
			continue;
		uint64_t len = _mmap[i].len, adr = _mmap[i].addr; 
		if(adr <= pks && adr+len >= pke) // kernel inside
		{
			if(adr+len-pke >= size+0x1000) // found!
				paddr = ((pke + 0xfffllu) & ~0xfffllu) + 0x1000;
		}
		else
		{
			if(len >= size + 0x1000) // todo right from here
				paddr = (adr + 0xfff) & ~0xfff;
		}
	}

	mprint("%llx", paddr);

	// 2. virtual memory
	// let's just map this to zero - it's a goddamn bitmap for memory management - a reasonable thing to hard map somewhere
	// also we'll unmap 0x100000-ish trampoline routines
	// a) unmapping

	{
		uint64_t vma = VMA_BBD_BITMAP;
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
		for(i = 3; (i > 1) && (*s & 1); i--)
			s = ((uint64_t)s << 9) | (p[i] << 3);

		for(; i > 1; i--)
		{
			uint64_t t = paddr;
			paddr += 0x1000;
			*s = 1 | t;
			s = (uint64_t)s << 9;
			s += p[i];
		}

		*s = paddr | 1;
		paddr += 0x1000;
		s = (uint64_t)s << 9;
		for(int i = 0; i<512; i++, *s++)
			*s = paddr | 1 + 0x1000*i;
		// *s = paddr | 1;
		// *s |= (1 << 7);
		pg_invtlb();
	}

	mprint("tada");

	return VMA_BBD_BITMAP;
}