#include <kernel/heap.h>
#include <stdint.h>
#include <kernel/consts.h>

static void* heap;
static void* p;

#define HB_USED (1 << 0)

#define HB_SIG 0xEDA02A2A

struct heapblock
{
	uint16_t size;
	uint16_t flags;
	uint32_t sig; // = HB_SIG

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
	for(struct heapblock* hb = HEAP_VMA_ADDR; hb < HEAP_VMA_ADDR+HEAP_SIZE; hb = (struct heapblock*)((char*)hb+HB_DEFBSIZE+sizeof(struct heapblock)))
	{
		printf("HEAP AT 0xffff8000%08x\n", hb);
		hb->sig = HB_SIG;
		hb->size = HB_DEFBSIZE;
	}
}

void* malloc(uint16_t size)
{
	struct heapblock* hb = p;
	while((hb->flags & HB_USED) && hb < HEAP_VMA_ADDR+HEAP_SIZE && hb->size >= size)
		hb += sizeof(hb) + hb->size;
	if(!((hb->flags & HB_USED) && hb < HEAP_VMA_ADDR+HEAP_SIZE)) return 0;
	hb->size = size;
	hb->flags |= HB_USED;
	hb->sig = HB_SIG;
	p = size;
}

void free(void* ptr)
{
	struct heapblock* hb = ptr;
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

// TODO:
// 1. basic functions
// * NB: break one free block into many
// 2. special cases, like 0x1000-aligned