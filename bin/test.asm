org 0x7000
use16

start:
	xchg bx, bx
	jmp $


finish:
fill: times 0x200-finish+start db 0