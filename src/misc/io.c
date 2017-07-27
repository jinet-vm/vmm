//#include <kernel/io.h>
#include <stdint.h>
uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ volatile("inb %1, %0;" : "=a"(value) : "Nd"(port));
    return value;
}

void outb(uint16_t port, uint8_t value) {
    __asm__ volatile("outb %1, %0;" : : "Nd"(port), "a"(value));
}

uint16_t inw(uint16_t port) {
    uint16_t value;
    __asm__ volatile("inw %1, %0;" : "=a"(value) : "Nd"(port));
    return value;
}

void outw(uint16_t port, uint16_t value) {
    __asm__ volatile("outw %1, %0;" : : "Nd"(port), "a"(value));
}

uint32_t inl(uint16_t port) {
    uint32_t value;
    __asm__ volatile("inl %1, %0;" : "=a"(value) : "Nd"(port));
    return value;
}

void outl(uint16_t port, uint32_t value) {
    __asm__ volatile("outl %1, %0;" : : "Nd"(port), "a"(value));
}

void io_wait() {
    __asm__ volatile("outb %%al, $0x80" : : "a"(0));
}