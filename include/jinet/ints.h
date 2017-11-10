#ifndef INTS_H
#define INTS_H

#include <stdint.h>

// struct regs
// {
// 	uint64_t ss, rsp, rflags, cs, rip, err_code, int_no,;  // dummy/proc pushed + our
// 	uint64_t rax, rbx, rcx, rdx, rbp, r8, r9, r10, r11, r12, r13, r14, r15; // GPR
// 	uint64_t rsi, rdi,; // Index regs
// } __attribute__((packed));

struct regs
{
	uint64_t rax, rbx, rcx, rdx, rbp, r8, r9, r10, r11, r12, r13, r14, r15; // GPR
	uint64_t rsi, rdi; // Index regs
	uint64_t int_no, err_code, rip, cs, rflags, userrsp, ss;
} __attribute__((packed));

void ints_install();

#define INTS_MAX_ISR 32
#define INTS_MAX_IRQ 40

#endif