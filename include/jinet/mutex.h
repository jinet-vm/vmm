#ifndef MUTEX_H
#define MUTEX_H

#include <stdint.h>

/** \typedef Binary semaphore; unlocked = 0; locked = 1 */
typedef uint8_t mutex;

#define DEFMUTEX(name) static mutex name = 0
#define MUTEX_IFLOCK(name) (name == 1)
#define MUTEX_LOCK(name) name = 1
#define MUTEX_UNLOCK(name) name = 0

void mutex_lock(mutex* m);
void mutex_unlock(mutex* m);

#endif