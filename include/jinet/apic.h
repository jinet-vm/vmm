#ifndef APIC_H
#define APIC_H

#include <stdint.h>

void lapic_setup();

#define LAPIC_ID_N 0x2
#define EOIR_N 0xB
#define SVR_N 0xF

#define LAPIC_REG_A_FIRST 0
#define LAPIC_REG_B_FIRST 1

uint32_t lapic_reg_read(int n);
void lapic_reg_write(int n, uint32_t val);
void lapic_eoi_send();
#endif