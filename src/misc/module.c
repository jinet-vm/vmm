#include <jinet/module.h>
#include <jinet/term.h>
#include <stdarg.h>

void _mprint(char* m, char* fmt, ...)
{
	va_list va;
	term_printk("\e[32;1m[%12s]\e[0m ", m);
	va_start(va, fmt);
	term_vprintk(fmt, va);
	va_end(va);
	term_printk("\n");
}