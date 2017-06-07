#ifndef TTY_H
#define TTY_H

#include <kernel/vga.h>

void tty_init();
void tty_setcolor(char color); // vga colors
void tty_puts(char* src);
void tty_putc(char a);


#define TTY_WIDTH 160
#define TTY_HEIGHT 64

#define VC_DEFAULT vga_color(VC_LIGHT_GREY,VC_BLACK)

#endif