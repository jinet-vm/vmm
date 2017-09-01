#ifndef INTS_H
#define INTS_H

#include <stdint.h>

struct regs
{
	//uint64_t r15, r14, r13, r12, r11, r10, r9, r8, rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax; // we push that
	//uint64_t gs, fs;
	uint64_t int_no, err_code, rip, cs, rflags, userrsp, ss; // dummy/proc pushed + our
} __attribute__((packed));

void ints_install();

#define INTS_MAX_ISR 32
#define INTS_MAX_IRQ 40

#endif