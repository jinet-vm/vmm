format elf
use32
extrn enterlm

; todo: multiboot 2

section '.multiboot' align 8

magic = 0x1BADB002
flags = 0x10006
chksm = -(magic+flags)

_start: ; something has gone wrong...
	xor eax, eax
	xor ebx, ebx
	jmp trump
align 4

multiboot_header:
	.magic:			dd magic				; required
	.flags:			dd flags				; required
	.checksum:		dd chksm				; required
	.header_addr:	dd multiboot_header	; if flags[16] is set
	.load_addr:		dd _start				; if flags[16] is set
	.load_end_addr:	dd 0					; if flags[16] is set
	.bss_end_addr:	dd 0					; if flags[16] is set
	.entry_addr:	dd trump				; if flags[16] is set
	.mode_type:		dd 0					; if flags[2] is set
	.width:			dd 1024					; if flags[2] is set
	.height:		dd 768				; if flags[2] is set
	.depth:			dd 32					; if flags[2] is set

section '.data' align 8

align 4
db 0,0
GDT:
d_zero:		db  0,0,0,0,0,0,0,0
d_code32:	db  0ffh,0ffh,0,0,0,10011010b,11001111b,0
d_data:		db	0ffh, 0ffh, 0x00, 0, 0, 10010010b, 11001111b, 0x00
GDTSize     =   $-GDT
times 5 db 0,0,0,0,0,0,0,0

GDTR:
g_size:     dw  GDTSize-1
g_base:     dd  GDT

section '.text' align 8

trump: ; 'cause trAmpoline
	cli
	cmp eax, 0x2BADB002
	jne $
	mov esp, theSTACK
	push ebx
	lgdt [GDTR]
	call enterlm ; enterlm(void* multiboot_table)

section '.bss' align 8

theSTACK: times 0x20000 db 0
actualesp: