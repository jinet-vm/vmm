#include <kernel/io.h>

unsigned char inb(unsigned short int port) {
    unsigned char value;
    __asm__ volatile("inb %1, %0;" : "=a"(value) : "Nd"(port));
    return value;
}

void outb(unsigned short int port, unsigned char value) {
    __asm__ volatile("outb %1, %0;" : : "Nd"(port), "a"(value));
}

void io_wait() {
    __asm__ volatile("outb %%al, $0x80" : : "a"(0));
}