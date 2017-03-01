#include <memory.h>

// src: toaruos
// todo: all the asm snippets - ?!?!?!?!
void* memset(void * dest, int c, size_t n) {
	asm volatile("rep stosb"
	             : "=c"((int){0})
	             : "D"(dest), "a"(c), "c"(n)
	             : "flags", "memory");
	return dest;
}