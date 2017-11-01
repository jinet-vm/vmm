#ifndef TSS_H
#define TSS_H


#include <stdint.h>

struct tss_entry_32 {
	uint32_t	prev_tss;
	uint32_t	esp0;
	uint32_t	ss0;
	uint32_t	esp1;
	uint32_t	ss1;
	uint32_t	esp2;
	uint32_t	ss2;
	uint32_t	cr3;
	uint32_t	eip;
	uint32_t	eflags;
	uint32_t	eax;
	uint32_t	ecx;
	uint32_t	edx;
	uint32_t	ebx;
	uint32_t	esp;
	uint32_t	ebp;
	uint32_t	esi;
	uint32_t	edi;
	uint32_t	es;
	uint32_t	cs;
	uint32_t	ss;
	uint32_t	ds;
	uint32_t	fs;
	uint32_t	gs;
	uint32_t	ldt;
	uint16_t	trap;
	uint16_t	iomap_base;
} __attribute__ ((packed));

struct tss_entry_64 {
	uint32_t _r0;
	uint64_t rsp[3];
	uint64_t _r1;
	uint64_t ist[7];
	uint64_t _r2;
	uint16_t _r3;
	uint16_t iomap_base;
} __attribute__ ((packed));

#endif