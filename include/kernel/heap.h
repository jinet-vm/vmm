#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>

// uint16_t instead of common size_t:
//  only blocks <= 0xffff are allowed

void heap_init();
void* malloc(uint16_t size);
void free(void* ptr);
void* realloc(void* ptr, uint16_t size);
void* calloc(uint16_t num, uint16_t size);

#endif