#ifndef CIRCBUF_H
#define CIRCBUF_H
#include <stdint.h>
typedef uint64_t Tval;

// TODO: do the private-public seperation?
struct circbuf
{
	int head;
	int tail;
	unsigned int count;
	unsigned int capacity;
	Tval *buffer;
};

struct circbuf circbuf_init(int capacity);
int circbuf_push(struct circbuf* b, Tval v);
int circbuf_count(struct circbuf* b);
Tval circbuf_pop(struct circbuf* b);

#endif