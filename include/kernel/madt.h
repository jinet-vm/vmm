#ifndef MADT_H
#define MADT_H

#include <stdint.h>

uint32_t detect_madt();
void detect_cpu_topology();
uint32_t madt_lapic_base();

#endif