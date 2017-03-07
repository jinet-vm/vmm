#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <kernel/irq.h>

void keyboard_handler(struct regs *r);

#endif