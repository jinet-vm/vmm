#ifndef MCFG_H
#define MCFG_H

#include <stdint.h>

#define MCFG_INVALID (uint64_t)(~0xF)

int mcfg_probe(void* table);
uint64_t mcfg_seg_group_count(void);
uint64_t mcfg_seg_group_bar(uint16_t sg);
uint64_t mcfg_seg_group_start_bus(uint16_t sg);
uint64_t mcfg_seg_group_end_bus(uint16_t sg);

#endif