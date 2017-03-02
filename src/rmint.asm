format elf

section '.text32' executable
use32

idt_real:
	dw 0x3ff
	dd 0

cr0_restore:
	dd 0

public rm_enter
rm_enter: ; must be stored in a 1:1 page
	cli