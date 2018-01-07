org 0x7000
use16

CR3_LM_PTR = 0x4000
AP_START_PTR = 0x4008
GDTP_LM_PTR = 0x4010

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

cr4_pae_bit = 00100000b
lm_msr = 0xC0000080
efer_lme = 0x00000100
cr0_bit = 0x80000000

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

entry_pm:
	cli
	mov ax, sel_data
	mov ss, ax
	mov ds, ax
	mov es, ax
	mov gs, ax
	mov fs, ax

	mov eax, cr4_pae_bit ; PAE 
	mov cr4, eax

	; xchg bx, bx
	mov edx, [CR3_LM_PTR]
	mov cr3, edx

	mov ecx, lm_msr
	rdmsr
	or eax, efer_lme
	wrmsr

	mov ebx, cr0
	or ebx, cr0_bit
	mov cr0, ebx

	;mov eax, [GDTP_LM_PTR]
	lgdt [GDTlm.Pointer]
	jmp 8:entry_lm
	
use64
entry_lm:
	mov rax, [AP_START_PTR]
	jmp rax
	jmp $

finish:
fill: times 0x4000-finish+start db 0