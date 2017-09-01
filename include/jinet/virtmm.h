#ifndef VIRTMM_H
#define VIRTMM_H

#include <stdint.h>

// recursive paging used here

void virtmm_init();
void* virtmm_alloc(uint64_t size);
void virtmm_free(uint64_t vma);

#endif