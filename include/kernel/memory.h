#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
void* memset(void * dest, int c, size_t n);
void* memcpy(void* restrict dstptr, const void* restrict srcptr, size_t size);

#endif