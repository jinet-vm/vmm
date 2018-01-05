#include <jinet/circbuf.h>


// inspiration: https://www.embedded.com/electronics-blogs/embedded-round-table/4419407/The-ring-buffer
// todo: have a data_struct folder?

struct circbuf circbuf_init(int capacity)
{
	struct circbuf b;
	b.head = 0;
	b.tail = 0;
	b.count = 0;
	b.capacity = capacity;
	b.buffer = malloc(sizeof(Tval)*capacity);
	return b;	
}

int circbuf_push(struct circbuf* b, Tval v)
{
	if(circbuf_full(b)) return 1; // full
	b->count++;
	b->buffer[b->head] = v;
	b->head = (b->head + 1) % b->capacity;
	if(b->head == b->tail)
		b->tail = (b->tail + 1) % b->capacity;
	return 0;
}

int circbuf_count(struct circbuf* b)
{
	return b->count; 	
}

Tval circbuf_pop(struct circbuf* b)
{
	if(circbuf_empty(b)) return 0;
	b->count--;
	int _t = b->buffer[b->tail];
	b->tail = (b->tail + 1) % b->capacity;
	return _t;
}

int circbuf_empty(struct circbuf* b)
{
	return (b->count == 0);
}

int circbuf_full(struct circbuf* b)
{
	return (b->count == b->capacity);
}