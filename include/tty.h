#ifndef TTY_H
#define TTY_H

#include <stddef.h>
#include <stdint.h>

void tty_init();
void tty_setcolor(uint8_t color);
void tty_print(char* src);

#endif