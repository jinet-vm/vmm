#ifndef IPI_H
#define IPI_H

#include <stdint.h>

#define DLM_FIXED 0
#define DLM_LOW 1
#define DLM_SMI 2
#define DLM_NMI 4
#define DLM_INIT 5
#define DLM_SIPI 6

#define DSM_PHYS 0
#define DSM_LOG 1

#define LVL_INIT 0
#define LVL_DEF 1

#define TRG_EDGE 0
#define TRG_LEVEL 1

#define DSH_NO 0
#define DSH_SELF 1
#define DSH_ALL_INSELF 2
#define DSH_ALL_EXSELF 3

void ipi_send(uint8_t vector, uint8_t delivery_mode, uint8_t dst_mode, uint8_t level, uint8_t trig, uint8_t dst_sh, uint8_t dst);

#endif