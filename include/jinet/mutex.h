#ifndef MUTEX_H
#define MUTEX_H

/** \typedef Binary semaphore; unlocked = 0; locked = 1 */
typedef uint8_t mutex;

#define DEFMUTEX(name) static mutex name = 0

void mutex_lock(mutex m);
void mutex_unlock(mutex m);

#endif