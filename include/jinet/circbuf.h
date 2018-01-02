#ifndef CIRCBUF_H
#define CIRCBUF_H

typedef T uint64_t;

// TODO: do the private-public seperation?
struct circbuf
{
	int head;
	int tail;
	int count;
	int capacity;
	T* buffer;
};

circbuf circbuf_init();
int circbuf_push(struct circbuf b, T);
int circbuf_count(struct circbuf b);
T circbuf_pop();

#endif