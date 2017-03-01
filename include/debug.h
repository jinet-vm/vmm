#ifndef DEBUG_H
#define DEBUG_H

#include <tty.h>
#include <vga.h>

#define DEBUG

#define mbp
#ifdef DEBUG
	#define mbp asm("xchgw %bx, %bx")
#endif

#define LOG(a) {tty_print(__FUNCTION__);tty_print(a);}
#define PANIC() LOG("PANIC");

#endif