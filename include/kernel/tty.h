#ifndef TTY_H
#define TTY_H

#include <stddef.h>
#include <stdint.h>
#include <kernel/vga.h>

void tty_init();
void tty_setcolor(uint8_t color); // vga colors
void tty_puts(char* src);

#endif