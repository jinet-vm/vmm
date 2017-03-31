#ifndef MADT_H
#define MADT_H

#include <stdint.h>

void detect_madt();
void detect_cpu_topology();
uint32_t madt_lapic_base();

#endif