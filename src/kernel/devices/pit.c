#include <jinet/pit.h>
#include <jinet/io.h>
#include <jinet/irq.h>
#include <jinet/ioapic.h>
#include <jinet/printf.h>

// hear me ROAR: http://forum.osdev.org/viewtopic.php?f=1&t=31329

// >> I/O Ports
#define PIT_COUNTER0 0x40
#define PIT_CMD 0x43

// >> Command Register (these are all shifted)
// > BCD
#define CMD_BINARY 0x00    // Use Binary counter values
#define CMD_BCD 0x01    // Use Binary Coded Decimal counter values

// > Mode
#define CMD_MODE0 0x00    // Interrupt on Terminal Count
#define CMD_MODE1 0x02    // Hardware Retriggerable One-Shot
#define CMD_MODE2 0x04    // Rate Generator
#define CMD_MODE3 0x06    // Square Wave
#define CMD_MODE4 0x08    // Software Trigerred Strobe
#define CMD_MODE5 0x0a    // Hardware Trigerred Strobe

// > Read/Write
#define CMD_LATCH 0x00
#define CMD_RW_LOW 0x10    // Least Significant Byte
#define CMD_RW_HIGH 0x20    // Most Significant Byte
#define CMD_RW_BOTH 0x30    // Least followed by Most Significant Byte

// > Counter Select
#define CMD_COUNTER0 0x00
#define CMD_COUNTER1 0x40
#define CMD_COUNTER2 0x80
#define CMD_READBACK 0xc0

void pit_irq();

void pit_init()
{
	// TODO: better? hardcoded; I want multitasking badly
	//idt_set_gate(32, (uint64_t)irq_sched, 0x08, 0x8E)
	irq_install_handler(0, pit_irq);
	pit_send_command(CMD_MODE3, CMD_RW_BOTH, CMD_COUNTER0);
	pit_set_freq(1000); // 1 ms
	ioapic_set_gate(2,32,0,0,0,0,0,0);
}

void pit_set_freq(uint16_t freq)
{
	uint16_t div = (uint16_t)(PIT_FREQ / freq);
	outb(PIT_COUNTER0, div & 0xff);
	outb(PIT_COUNTER0, div >> 8);
}

void pit_send_command(uint8_t opmod, uint8_t acmod, uint8_t selchan)
{
	uint8_t command = 0;
	command |= opmod & 0xE;
	command |= acmod & 0x30;
	command |= selchan & 0xC0;
	outb(PIT_CMD, command);
}

static uint64_t count = 0;

void pit_irq(struct regs *r)
{
	//if(count % 1000 == 0) printf("a");
	count++;
}

void pit_sleep(uint64_t ms)
{
	static uint64_t end;
	end = count + ms;
	while(1){
		if(count >= end) break;
	}
}