#include <kernel/module.h>
#include <kernel/term.h>
#include <stdarg.h>

void _mprint(char* m, char* fmt, ...)
{
	va_list va;
	term_printk("[%8s] ", m);
	va_start(va, fmt);
	term_vprintk(fmt, va);
	va_end(va);
	term_printk("\n");
}