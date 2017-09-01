#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <jinet/irq.h>

void keyboard_handler(struct regs *r);
void serial_handler(struct regs *r);
#endif