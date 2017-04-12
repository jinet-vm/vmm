#ifndef IOAPIC_H
#define IOAPIC_H

#include <stdint.h>

void ioapic_setup();
uint32_t ioapic_reg_read(uint8_t n);
void ioapic_reg_write(uint8_t n, uint32_t val);

void ioapic_set_gate(uint8_t n, uint8_t intvec, uint8_t delmod, uint8_t destmod, uint8_t intpol, uint8_t trigmod, uint8_t intmask, uint8_t dest);

#endif