#include "basicmm.h"
#include <stdint.h>

#define BASICMM_MAXN 20

typedef struct
{
	uint32_t begin;
	uint32_t end;
} reg; // region lasts from [begin, end)

static reg R[BASICMM_MAXN];
static int ri = 0;

void basicmm_add(uint32_t begin, uint32_t end)
{
	if(ri >= BASICMM_MAXN) return;
	R[ri].begin = begin;
	R[ri++].end = begin+length+1;
}

int basicmm_check(uint32_t r)
{
	for(int i = 0; i<BASICMM_MAXN; i++)
		if((R[i].begin <= r) && (r <= R[i].end))
			return 1;
	return 0;
}

// we suppose the removed region resides fully inside one reg
int basicmm_rm(uint32_t begin, uint32_t size)
{
	int t = -1;
	for(int i = 0; i<ri; i++)
		if(R[i].begin <= begin && begin <= R[i].end)
		{
			t = i;
			break;
		}
	if(t == -1) return 1;
	int tb, te;
	tb = R[i].begin; te = R[i].end;
	reg a = {.begin = tb, .end = begin}, b = {.begin = begin+size, .end = te};
	if(a.begin < a.end && b.begin < b.end) {R[t] = a; basicmm_add(b.begin);}
	else if(a.begin < a.end) R[t] = a;
	else if(b.begin < b.end) R[t] = b;
	return 0;
}

void* basicmm_alloc(uint32_t size)
{
	for(int i = 0; i<ri; i++)
	{
		if(R[i].end - R[i].begin >= size)
		{
			R[i].begin += size;
			return R[i].begin - size;
		}
	}
	return 0;
}