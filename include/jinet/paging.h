#ifndef PAGING_H
#define PAGING_H
#include <stdint.h>
#include <stddef.h>

uint64_t pg_get_paddr(uint64_t vma);
void pg_invtlb();
void pg_map(uint64_t vma, uint64_t paddr, int order);
int pg_map_reg(uint64_t vma, uint64_t paddr, uint64_t size);

#endif