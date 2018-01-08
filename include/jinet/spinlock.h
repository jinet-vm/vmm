#ifndef SPINLOCK_H
#define SPINLOCK_H

typedef char spinlock;

#define DEFSPINLOCK(name) volatile spinlock name

// todo: is it ok to use gcc-specific stuff here?
// todo: what the hell is __sync_synchronize() ?

#define SPINLOCK_LOCK(name) \
	while (!__sync_bool_compare_and_swap(&name, 0, 1)); \
	__sync_synchronize();

#define SPINLOCK_UNLOCK(name) \
	__sync_synchronize(); \
	name = 0;

#endif