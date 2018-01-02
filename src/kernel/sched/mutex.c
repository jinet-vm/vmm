#include <jinet/mutex.h>

void mutex_lock(mutex m)
{
	while(m) {;}
	m = 1;
}

void mutex_unlock(mutex m)
{
	m = 0;
}