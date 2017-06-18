/**
 * @file memory.c
 * Low-level memory functions.
 */

#include <kernel/memory.h>

/**
 * @brief      Sets memory.
 *
 * @param      dest  The destination
 * @param[in]  c     Value to set
 * @param[in]  n     Number of bytes to be set.
 *
 * @return     End destination
 */
void* memset(void * dest, int c, size_t n) {
	asm volatile("rep stosb"
	             : "=c"((int){0})
	             : "D"(dest), "a"(c), "c"(n)
	             : "flags", "memory");
	return dest;
}

void * memcpy(void * restrict dest, const void * restrict src, size_t count) {
	asm volatile ("cld; rep movsb" : "+c" (count), "+S" (src), "+D" (dest) :: "memory");
	return dest;
}