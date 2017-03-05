format elf

section '.text' executable

public isr_default_handler
isr_default_handler:
	pushad
	; TODO: do flip-flop with segments
	;call ints_handler ; defined in C!
	nop
	nop
	popad
	;add esp, 8 ; isr error code & irq number
	iret