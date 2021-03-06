#include <jinet/term.h>
#include <jinet/printf.h>
#include <jinet/module.h>
#include <stdarg.h>

static struct term_dev terms[MAXTERMS];
static unsigned char termsI = 0;

static struct termbuf* tb[MAXTERMBUFS];
static struct termbuf* curtb;

int tbn = 0;

int term_init()
{
	curtb = tb[0];
	init_printf(NULL, term_putc);
	return 0;
}

int term_add(struct term_dev t)
{
	int r = t.init(&t);
	mprint("addr: 0x%08x",t.addr);
	if(!r) // all good
		terms[termsI++] = t;
	#ifdef TERM_ADD_RES_PRINT
	if(!r)
		mprint("Terminal %s successfully initialized", t.name);
	else
		mprint("Terminal %s failed to initialize", t.name);
	#endif
	return r;
}

void term_write(char *s, size_t l)
{
	char *c = s;
	for(unsigned char ti = 0; ti < termsI; ti++)
		for(int i = 0; i<l; i++)
			terms[ti].putc(&terms[ti], s[i]);
}

void term_print(char *s)
{
	char *c = s;
	while(*c != 0)
		term_putc(NULL, *c++);
}

void term_putc(void *s, char c)
{
	for(unsigned char ti = 0; ti < termsI; ti++)
		terms[ti].putc(&terms[ti], c);
}

int term_vprintk(char *fmt, va_list va)
{
	tfp_vprintf(fmt, va);
}

int term_printk(char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	tfp_vprintf(fmt, va);
	va_end(va);
}

int term_task()
{
	for(;;)
	{
		for(int i = 0; i<tbn; i++)
		{

		}
	}
}