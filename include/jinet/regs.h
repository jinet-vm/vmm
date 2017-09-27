#ifndef REGS_H
#define REGS_H

#include <stdint.h>

uint64_t cr0_get();
void cr0_set(uint64_t value);

uint64_t cr4_get();
void cr4_set(uint64_t value);

uint64_t cr3_get();

uint16_t cs_get();
uint16_t ds_get();
uint16_t fs_get();
uint16_t es_get();
uint16_t gs_get();

void cs_set(uint16_t seg);
void ds_set(uint16_t seg);
void fs_set(uint16_t seg);
void es_set(uint16_t seg);
void gs_set(uint16_t seg);

uint16_t tr_get();
void tr_set(uint16_t val);
uint16_t lar(uint16_t seg);

#endif