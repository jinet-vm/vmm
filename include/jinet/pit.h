#ifndef PIT_H
#define PIT_H

#define PIT_FREQ 1193182

#include <stdint.h>

void pit_init();
void pit_sleep(uint64_t ms);

#endif