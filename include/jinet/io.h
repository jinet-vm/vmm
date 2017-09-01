#ifndef IO_H
#define IO_H

#include <stdint.h>

uint8_t inb(uint32_t port);
void outb(uint32_t port, uint8_t value);

uint16_t inw(uint32_t port);
void outw(uint32_t port, uint16_t value);

uint32_t inl(uint32_t port);
void outl(uint32_t port, uint32_t value);

void io_wait();

#endif