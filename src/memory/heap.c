#include <jinet/heap.h>
#include <stdint.h>
#include <jinet/consts.h>
#include <jinet/printf.h>

static void* heap; // stores heap pointer
static void* p; // stores pointer to the initial place of search for free blocks

#define HB_USED (1 << 0)

#define HB_SIG 0x2a2a

// TODO: delimiter
struct heapblock
{
	uint32_t size;
	uint16_t flags;
	uint16_t sig; // = HB_SIG

	/*
	heapblock flags:

	bit 0 - used (1) or available (0)
	bit 1:15 - reserved

	*/

} __attribute__ ((packed));

#define HB_DEFBSIZE (0x10000-sizeof(struct heapblock))

void heap_init()
{
	heap = HEAP_VMA_ADDR;
	p = HEAP_VMA_ADDR;
	struct heapblock* hb = heap;
	hb->sig = HB_SIG;
	hb->size = HEAP_SIZE-sizeof(struct heapblock);
	//heap_show_blocks();
}

void* malloc(uint16_t size)
{
	struct heapblock* hb = heap;
	while((hb->flags & HB_USED) && hb < HEAP_VMA_ADDR+HEAP_SIZE && hb->size >= size)
		hb = (struct heapblock*)((char*)hb + sizeof(hb) + hb->size);
	if(((hb->flags & HB_USED) && hb < HEAP_VMA_ADDR+HEAP_SIZE && hb->size >= size)) return 0;
	// init this hb (used)
	//printf("0xffff8000%08x\n",hb);
	uint32_t prev_size = hb->size;
	hb->size = size;
	hb->flags |= HB_USED;
	hb->sig = HB_SIG;
	void* ret = (char*)hb+sizeof(hb);
	// init next (free)
	if(prev_size > size+sizeof(struct heapblock))
	{
		hb = (struct heapblock*)((char*)hb+size+sizeof(struct heapblock));
		hb->size = prev_size-(size+sizeof(struct heapblock));
		hb->flags = 0;
		hb->sig = HB_SIG;
	}
	//p = hb+1;
	return ret;
}

void free(void* ptr)
{
	struct heapblock* hb = ptr - sizeof(struct heapblock);
	if(hb->sig != HB_SIG) return;
	hb->flags &= ~HB_USED;
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
	for(;hb < heap+HEAP_SIZE; hb = (struct heapblock*)((char*)hb + hb->size + sizeof(struct heapblock)))
	{
		char* state = "free";
		if(hb->flags & HB_USED)
			state = "used";
		else
			state = "free";
		printf("hb (%s) at 0x%08x%08x\nsize = 0x%x bytes\n", state, ((uint64_t)((char*)hb+sizeof(struct heapblock))) >> 32, ((char*)hb+sizeof(struct heapblock)), hb->size);
	}
	printf("                              \n");
}

// TODO:
// 1. basic functions
// * NB: break one free block into many
// 2. special cases, like 0x1000-aligned