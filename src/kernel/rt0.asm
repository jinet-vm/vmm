format elf64
use64

section '.text' executable
extrn kernel_start

public rt0
rt0:
	xchg bx, bx
	jmp kernel_start