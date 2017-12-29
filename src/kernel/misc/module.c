#include <jinet/module.h>
#include <jinet/term.h>
#include <stdarg.h>

void _mprint(char* file, int line, char* fmt, ...)
{
	va_list va;
	term_printk("\e[32;1m%s:%03d\e[0m ", file, line);
	va_start(va, fmt);
	term_vprintk(fmt, va);
	va_end(va);
	term_printk("\n");
}