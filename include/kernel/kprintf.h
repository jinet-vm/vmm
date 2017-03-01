#ifndef KPRINTF_H
#define KPRINTF_H

#include <stdarg.h>
#include <stdint.h>

//void putchar(int c, void *arg);
void kprintf(const char *fmt, ...);
//char * ksprintn(char *nbuf, uintmax_t num, int base, int *lenp, int upper);
int kvprintf(char const *fmt, void (*func)(int, void*), void *arg, int radix, va_list ap);

#endif