#ifndef INTS_H
#define INTS_H

struct regs
{
	unsigned int gs, fs;
	unsigned int rdi, rsi, rbp, rsp, rbx, rdx, rcx, rax;
	unsigned int int_no, err_code;
	unsigned int rip, cs, rflags, useresp, ss;
};

void ints_install();

#define INTS_MAX_ISR 32
#define INTS_MAX_IRQ 40

#endif