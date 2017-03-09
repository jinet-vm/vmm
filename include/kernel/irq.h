#ifndef IRQ_H
#define IRQ_H

#include <kernel/ints.h>

void irq_install();
void irq_install_handler(int irq, void (*handler)(struct regs*));

#endif