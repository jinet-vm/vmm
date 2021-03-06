#ifndef PHYSMM_H
#define PHYSMM_H

#include <stdint.h>
#include <jinet/multiboot2.h>

void physmm_init(struct multiboot_mmap_entry* mmap, int length);
uint64_t physmm_alloc(int order);
void physmm_free(uint64_t paddr, int order);

#endif