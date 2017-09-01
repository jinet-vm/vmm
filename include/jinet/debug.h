#ifndef DEBUG_H
#define DEBUG_H

#include <jinet/tty.h>
#include <jinet/vga.h>

//#define DEBUG 1

#define mbp
#ifdef DEBUG
	#define mbp asm("xchgw %bx, %bx")
#endif

#define LOG(a) {tty_print(__FUNCTION__);tty_print(a);}
#define PANIC() LOG("PANIC");

#endif