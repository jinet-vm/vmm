#ifndef IPI_H
#define IPI_H

#include <stdint.h>

void ipi_send(uint8_t vector, uint8_t delivery_mode, uint8_t dst_mode, uint8_t level, uint8_t trig, uint8_t dst_sh, uint8_t dst);

#endif