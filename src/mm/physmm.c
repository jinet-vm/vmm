#include <jinet/physmm.h>
#include <jinet/module.h>

MODULE("PHYSMM");

static uint8_t* bitmap[11];
static uint64_t boffs[11];
static uint64_t _ts;

// basic_alloc is used only ONCE
static int bbd_init(uint64_t total_size, void* (*basic_alloc)(uint64_t))
{
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

	bitmap[0] = (uint8_t*) basic_alloc(compmal);
	if(!bitmap[0]) return 0;
	for(int i = 1; i < 10; i++)
		bitmap[i] = bitmap[0] + boffs[i];
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
		uint64_t page = 0x1000, porder = 0;
		for(page = 0x1000, porder = 0;
			(p + (page << 1)) <= (addr+size) && !(p & ((page << 1) - 1));
			page <<= 1, porder++);
		bbd_free_page(p, porder);
		p += page;
	}
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
	int o = order;
	uint64_t block = 0;
	while(o < 10)
	{
		uint64_t page = 0x1000 << o;
		uint64_t size = (_ts + page - 1) / page;
		uint64_t i;
		for(i = 0; i < _ts; i+=page)
			if(bbd_get_page(i, o))
				break;
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

struct multiboot_mmap_entry* _mmap;
int _num;

void* basic_alloc(uint64_t size);

void physmm_init(struct multiboot_mmap_entry* mmap, int num)
{
	_mmap = mmap;
	_num = num;
	uint64_t ts = 0;
	for(int i = 0; i < num; i++)
		if(ts < mmap[i].addr + mmap[i].len)
			ts = mmap[i].addr + mmap[i].len;
	bbd_init(ts, &basic_alloc);
	for(int i = 0; i<num; i++)
		if(mmap[i].type == MULTIBOOT_MEMORY_AVAILABLE)
			bbd_add_region(mmap[i].addr, mmap[i].len);
}

// this thing does a lot of things
// 1. find physical memory (_size_)
// 2. map it somewhere, initialize it (?) and return
void* basic_alloc(uint64_t size)
{
	
}