#ifndef PAGING_H
#define PAGING_H
#include <stdint.h>
#include <stddef.h>

uint64_t pg_get_paddr(uint64_t vma);
void pg_invtbl();
void pg_map(uint64_t vma, uint64_t paddr, int order);

#endif