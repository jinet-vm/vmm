#include <jinet/ipi.h>
#include <jinet/apic.h>
#include <jinet/debug.h>

#define ICR_LOW 0x30
#define ICR_HIGH 0x31

void ipi_send(uint8_t vector, uint8_t delivery_mode, uint8_t dst_mode, uint8_t level, uint8_t trig, uint8_t dst_sh, uint8_t dst)
{
	// todo: discover a better way
	// to store bit offsets than
	// just in defines (!)
	// todo: probably, packed struct and uint32_t pointers
	uint32_t ICR_low = 0, ICR_high = 0;
	ICR_low |= vector;
	ICR_low |= (delivery_mode & 7) << 8;
	ICR_low |= (dst_mode & 1) << 11;
	ICR_low |= (level & 1) << 14;
	ICR_low |= (trig & 1) << 15;
	ICR_low |= (dst_sh & 3) << 18;
	ICR_high |= dst << 56;
	lapic_reg_write(ICR_HIGH, ICR_high);
	lapic_reg_write(ICR_LOW, ICR_low);
	// god save our souls
}