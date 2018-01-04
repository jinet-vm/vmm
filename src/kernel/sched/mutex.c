#include <jinet/mutex.h>

void mutex_lock(mutex* m)
{
	while(MUTEX_IFLOCK(*m)) noirq_sched();
	MUTEX_LOCK(*m);
}

void mutex_unlock(mutex* m)
{
	MUTEX_UNLOCK(*m);
}