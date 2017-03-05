#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#define IDT_ENTRIES 256

struct idt_header {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

struct idt_entry {
	uint16_t offset_low;
	uint16_t selector;
	uint8_t always0;
	uint8_t flags;
	uint16_t offset_high;
} __attribute__((packed));

void idt_init();
void idt_set_gate(char i, uint32_t off, uint16_t sel, uint8_t flags);
void idt_flush();

#endif IDT_H