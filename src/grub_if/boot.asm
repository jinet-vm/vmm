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

align 8
GDTlm:
	dq 0x0000000000000000             ; Null Descriptor - should be present.
	dq 0x00209A0000000000             ; 64-bit code descriptor (exec/read).
	dq 0x0020920000000000             ; 64-bit data descriptor (read/write).
 
align 4
	dw 0                              ; Padding to make the "address of the GDT" field aligned on a 4-byte boundary
 
.Pointer:
	dw $ - GDTlm - 1                    ; 16-bit Size (Limit) of GDT.
	dd GDTlm                            ; 32-bit Base Address of GDT. (CPU will zero extend to 64-bit)

extrn future_cr3
extrn ent

trump: ; 'cause trAmpoline
	; xchg bx, bx
	cli
	cmp eax, 0x36d76289
	jne $
	mov esp, theSTACK
	push ebx
	lgdt [GDTR]
	call enterlm ; enterlm(void* multiboot_table)

	; xchg bx, bx

cr4_pae_bit = 00100000b
lm_msr = 0xC0000080
efer_lme = 0x00000100
cr0_bit = 0x80000000
	; todo: what is happening here? UPD: messing with bootstruct probably
	; todo: bootstruct fasm struc !
	mov eax, [ent]
	mov [rent], eax
	mov eax, [ent+4]
	mov [rent+4], eax

	mov eax, cr4_pae_bit ; PAE 
	mov cr4, eax

	; xchg bx, bx
	mov edx, [future_cr3]
	mov cr3, edx

	mov ecx, lm_msr
	rdmsr
	or eax, efer_lme
	wrmsr

	mov ebx, cr0
	or ebx, cr0_bit
	mov cr0, ebx

	lgdt [GDTlm.Pointer]
	mov eax, rent
	jmp 8:lm

align 8
dd 0
lm:

include 'inc/consts.inc'
use64
	; xchg bx, bx
	; mov rax, [rax]
	; jmp rax

	; rsi - rent

	;file 'lm.bin'
	mov rsp, KERNEL_STACK_VMA_ADDR-0x1000 ; todo: bootstruct way?
	mov rax, [rax]
	jmp rax
	; it's PICy

rent: dq ent ; offset
section '.bss' align 8

theSTACK: times 0x4000 db 0
actualesp: