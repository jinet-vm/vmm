#ifndef MADT_H
#define MADT_H

#include <stdint.h>

#define MADT_OUTPUT	

int madt_probe(void* tbl);
void madt_topology_detect();
uint32_t madt_lapic_base();

#endif