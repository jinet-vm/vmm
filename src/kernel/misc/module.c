#include <jinet/module.h>
#include <jinet/term.h>
#include <stdarg.h>
#include <jinet/spinlock.h>

DEFSPINLOCK(print_s);

void _mprint(char* file, int line, char* fmt, ...)
{
	SPINLOCK_LOCK(print_s);
	va_list va;
	term_printk("\e[32;1m%s:%03d\e[0m ", file, line);
	va_start(va, fmt);
	term_vprintk(fmt, va);
	va_end(va);
	term_printk("\n");
	SPINLOCK_UNLOCK(print_s);
}