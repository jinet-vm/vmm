org 0x4000
use16

start:
	cli
	xchg bx, bx
	; enter PM
	lgdt fword [GDTR]
	mov eax, cr0
	or al, 1
	mov cr0, eax

	; O32 jmp far
	db  66h ; O32
	db  0eah ; JMP FAR
	dd  entry_pm ; offset
	dw  sel_code32 ; selector
	jmp $ ; in case something went wrong

GDTTable:   ;таблица GDT
; zero seg
d_zero:		db  0,0,0,0,0,0,0,0     
; 32 bit code seg
d_code32:	db  0ffh,0ffh,0,0,0,10011010b,11001111b,0
; data
d_data:		db	0ffh, 0ffh, 0x00, 0, 0, 10010010b, 11001111b, 0x00
GDTSize     =   $-GDTTable
times 2 db 0,0,0,0,0,0,0,0

GDTR:
g_size:     dw  GDTSize-1
g_base:     dd  GDTTable

sel_code32	= 0x8
sel_data	= 0x10

use32

entry_pm:
	cli
	mov ax, sel_data
	mov ss, ax
	mov ds, ax
	mov es, ax
	mov gs, ax
	mov fs, ax
	mov esp, 0x7000
	mov eax, 0
	.lp:
	mov ebx, [0x7880]
	test ebx, ebx
	jz .lp
	mov edi, 0x7800
	stosd
	inc eax
	mov edi, 0x7880
	mov ebx, 1
	mov [edi], ebx
	jmp .lp

finish:
fill: times 0x4000-finish+start db 0