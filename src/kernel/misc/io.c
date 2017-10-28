#include <jinet/io.h>
#include <stdint.h>

uint8_t inb(uint32_t port) {
    uint8_t value;
    __asm__ volatile("inb %w1, %0;" : "=a"(value) : "Nd"(port));
    return value;
}

void outb(uint32_t port, uint8_t value) {
    __asm__ volatile("outb %1, %w0;" : : "Nd"(port), "a"(value));
}

uint16_t inw(uint32_t port) {
    uint16_t value;
    __asm__ volatile("inw %w1, %0;" : "=a"(value) : "Nd"(port));
    return value;
}

void outw(uint32_t port, uint16_t value) {
    __asm__ volatile("outw %1, %w0;" : : "Nd"(port), "a"(value));
}

uint32_t inl(uint32_t port) {
    uint32_t value;
    __asm__ volatile("inl %w1, %0;" : "=a"(value) : "Nd"(port));
    return value;
}

void outl(uint32_t port, uint32_t value) {
    __asm__ volatile("outl %1, %w0;" : : "Nd"(port), "a"(value));
}

void io_wait() {
    __asm__ volatile("outb %%al, $0x80" : : "a"(0));
}