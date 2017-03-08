#ifndef INTS_H
#define INTS_H

struct regs
{
	unsigned int gs, fs, es, ds;
	unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
	unsigned int int_no, err_code;
	unsigned int eip, cs, eflags, useresp, ss;
};

void ints_install();

#define INTS_MAX_ISR 32
#define INTS_MAX_IRQ 40

#endif