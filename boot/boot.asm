format elf
use32
extrn enterlm

; todo: multiboot 2

section '.multiboot' align 8

public multiboot2_header

align 8
multiboot2_header:
	.magic: dd 0xe85250d6
	.arch: dd 0
	.length: dd multiboot2_header_end - multiboot2_header
	.checksum: dd -(0xe85250d6 + 0 + (multiboot2_header_end - multiboot2_header))

	; align 8
	; mbh_entry:
	; 	.type: dw 3
	; 	.flags: dw 0
	; 	.size: dw 12
	; 	.entry_addr: dq trump
	; mbh_entry_end:

	align 8
	mbh_info_request:
		.type: dw 1
		.flags: dw 0
		.size: dd mbh_info_request_end - mbh_info_request
		.tags: dd 4, 6, 8
	mbh_info_request_end:

	align 8
	mbh_fb:
		.type: dw 5
		.flags: dw 0
		.size: dd 20
		.width: dd 0
		.height: dd 0
		.depth: dd 0 
	mbh_fb_end:

	align 8
	mbh_modalign:
		.type: dw 6
		.flags: dw 0
		.size: dw 8
	mbh_modalign_end:

	align 8
	mbh_end:
		.type: dw 0
		.flags: dw 0
		.size: dd 8

multiboot2_header_end:
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

public trump

trump: ; 'cause trAmpoline
	cli
	cmp eax, 0x36d76289
	jne $
	mov esp, theSTACK
	push ebx
	lgdt [GDTR]
	call enterlm ; enterlm(void* multiboot_table)

section '.bss' align 8

theSTACK: times 0x4000 db 0
actualesp: