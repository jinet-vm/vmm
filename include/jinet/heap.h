#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>

void heap_init();
void heap_show_blocks();
void* malloc(uint64_t size);
void free(void* ptr);
void* realloc(void* ptr, uint64_t size);
void* calloc(uint16_t num, uint64_t size);

#endif