#include <jinet/heap.h>
#include <stdint.h>
#include <jinet/vmaddr.h>
#include <jinet/module.h>
#include <jinet/paging.h>
#include <jinet/mutex.h>
MODULE("HEAP");

static void* heap; // heap pointer
static void* p; // stores pointer to the initial place of search for free blocks

#define HB_USED (1 << 0)
#define HB_SIG 0xcafe

// TODO: delimiter
struct heapblock
{
	uint16_t sig; // = HB_SIG
	uint32_t size;
	uint16_t flags;

	/*
	heapblock flags:

	bit 0 - used (1) or available (0)
	bit 1:15 - reserved

	*/

} __attribute__ ((packed));

void heap_init()
{
	uint64_t s = physmm_alloc(4);
	mprint("%llx", s);
	pg_map_reg(VMA_HEAP, s, 0x10000);
	mprint("tada");
	heap = VMA_HEAP;
	p = VMA_HEAP;
	struct heapblock* hb = heap;
	hb->sig = HB_SIG;
	hb->size = VMA_HEAP_SIZE-sizeof(struct heapblock);
	//heap_show_blocks();
}

mutex alloc_mutex;

// TODO: mutex?
void* malloc(uint16_t size)
{
	mutex_lock(&alloc_mutex);
	struct heapblock* hb = heap;
	while((hb->flags & HB_USED) && hb < VMA_HEAP+VMA_HEAP_SIZE && hb->size >= size)
		hb = (struct heapblock*)((char*)hb + sizeof(hb) + hb->size);
	if(((hb->flags & HB_USED) && hb < VMA_HEAP+VMA_HEAP_SIZE && hb->size >= size)) return 0;
	uint32_t prev_size = hb->size;
	hb->size = size;
	hb->flags |= HB_USED;
	hb->sig = HB_SIG;
	void* ret = (char*)hb+sizeof(hb);
	if(prev_size > size+sizeof(struct heapblock))
	{
		hb = (struct heapblock*)((char*)hb+size+sizeof(struct heapblock));
		hb->size = prev_size-(size+sizeof(struct heapblock));
		hb->flags = 0;
		hb->sig = HB_SIG;
	}
	mutex_unlock(&alloc_mutex);
	return ret;
}

DEFMUTEX(free_mutex);

void free(void* ptr)
{
	//mutex_lock(&free_mutex);
	struct heapblock* hb = ptr - sizeof(struct heapblock);
	if(hb->sig != HB_SIG) return;
	hb->flags &= ~HB_USED;
	//mutex_unlock(&free_mutex);
}

void* realloc(void* ptr, uint16_t size)
{
	// TODO: not implemented
}

void* calloc(uint16_t num, uint16_t size)
{
	// TODO: not implemented
}

void heap_show_blocks()
{
	struct heapblock* hb = heap;
	for(;hb < heap+VMA_HEAP_SIZE; hb = (struct heapblock*)((char*)hb + hb->size + sizeof(struct heapblock)))
	{
		char* state = "free";
		if(hb->flags & HB_USED)
			state = "used";
		else
			state = "free";
		mprint("hb (%s) at 0x%08x%08x: size = 0x%x bytes", state, ((uint64_t)((char*)hb+sizeof(struct heapblock))) >> 32, ((char*)hb+sizeof(struct heapblock)), hb->size);
	}
}