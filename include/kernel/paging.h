#ifndef PAGING_H
#define PAGING_H
#include <stdint.h>
#include <stddef.h>
#define p_entry(addr, f) (addr << 12) | f

#define PD PD_a
#define PDE(i) PD+4*i
#define PT(i) PD+0x1000+0x1000*((void*)i)
#define PTE(n, i) PT(n)+4*i

#define pg_P 1
#define pg_R 2
#define pg_U 4

#pragma pack(push, 1)
typedef struct mmap_entry
{
	int base_low;
	int base_high;
	int length_low;
	int length_high;
	int type;
} mmap_entry;
#pragma pack(pop)

void* init_PD(); // returns PD address
void get_available_memory();
size_t map_available_memory(); // returns available memory
void map_page(uint32_t src, uint32_t dst, uint8_t flags);
void init_paging();
uint32_t get_paddr(uint32_t laddr);

#endif