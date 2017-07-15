#include <kernel/term.h>

static struct term_dev terms[MAXTERMS];
static unsigned char termsI = 0;

int term_add(struct term_dev t)
{
	const char w1[] = "[  TERM] Terminal ", w2[] = " initialized\n", w3[] = " failed to initialize\n";
	int r = t.init(&t);
	if(!r) // all good
		terms[termsI++] = t;
	
	#ifdef TERM_ADD_RES_PRINT
	term_print(w1);
	term_print(t.name);
	if(!r)
		term_print(w2);
	else
		term_print(w3);
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
	for(unsigned char ti = 0; ti < termsI; ti++)
		while(*c != 0)
			terms[ti].putc(&terms[ti], *c++);
}