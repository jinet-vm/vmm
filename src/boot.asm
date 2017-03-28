format ELF

include 'inc/macro.inc'

; >>>> 16bit code

section '.text16' executable
Use16

org 0x7c00 ; why?! loop problems

public start
start:
	;mbp
	cli		     ; disabling interrupts
	mov     ax, cs	  ; segment registers' init
	mov     ds, ax
	mov     es, ax
	mov     ss, ax
	mov     sp, 0x7C00      ; stack backwards => ok

	shl eax,4       ;умножаем на 16
	mov ebx,eax     ;копируем в регистр EBX
	; why?!

	push dx, bx, ax, cx
	mov dx, 0 ; set cursor to top left-most corner of screen
	mov bh, 0 ; page
	mov ah, 0x2 ; ah = 2 => set cursor
	int 0x10 ; moving cursor
	mov cx, 2000 ; print 2000 = 80*45 chars
	mov bh, 0
	mov ah, 0x9
	int 0x10
	pop cx, ax, bx, dx

	; mbp
	; ; loading entry_pm to RAM
	; mov ah, 0x02    ; Read Disk Sectors
	; mov al, 19    ; Read one sector only (512 bytes per sector)
	; mov ch, 0x00    ; Track 0
	; mov cl, 0x02    ; Sector 2
	; mov dh, 0x00    ; Head 0
	; mov dl, 0x00    ; Drive 0 (Floppy 1)
	; mov bx, cs
	; mov es, bx   ; Segment 0x2000
	; mov bx, 0x7e00      ;  again remember segments but must be loaded from non immediate data
	; int 13h
	;mbp
	mov si, DAP
	mov ah, 0x42
	mov dl, 0x80 ; Floppy
	int 0x13
	
	;mbp
	; memory map
memory_map:
	xor ebx, ebx
	xor bp, bp
	mov edx, 534D4150h
	mov eax, 0xe820
	mov edi, 0xF000-20
	.lp:
		add edi, 20
		mov ecx, 20
		mov edx, 534D4150h
		mov eax, 0xe820
		int 15h
		test ebx, ebx
	jnz .lp

	;mbp

	; loading GDT
	lgdt    fword   [GDTR]

	; disable NMI
	in  al,70h
	or  al,80h
	out 70h,al

	; enable a20
	in  al,92h
	or  al,2
	out 92h,al
	
	; get into PM
	mov eax,cr0
	or  al,1     
	mov cr0,eax
	;mbp
	; O32 jmp far
	db  66h ; O32
	db  0eah ; JMP FAR
	dd  entry_pm ; offset
	dw  sel_code32 ; selector

DAP:
	.size:	db 10h
	.zero:	db 0
	.num:	dw 100
	.addr:	dw 0x7e00
			dw 0
	.lba:	dd 1
			dd 0

; the same is done in desc.asm - for better migration to >1MB memspace

GDTTable:   ;таблица GDT
; zero seg
d_zero:		db  0,0,0,0,0,0,0,0     
; 32 bit code seg
d_code32:	db  0ffh,0ffh,0,0,0,10011010b,11001111b,0
; data
d_data:		db	0ffh, 0ffh, 0x00, 0, 0, 10010010b, 11001111b, 0x00
GDTSize     =   $-GDTTable
times 5 db 0,0,0,0,0,0,0,0

GDTR:
g_size:     dw  GDTSize-1
g_base:     dd  GDTTable

; >>>> 32bit code

section '.text32' executable align 100h
; org     0x7E00 - done by ld
use32 ; 32 bit PM

public entry_pm

extrn kernel_start
; include 'consts.ld'

; PAGING_PHYS_ADDR:
; <PDPT>

align   10h         ;код должен выравниваться по границе 16 байт
; include 'inc/procedures.inc'

extrn LongMode

; _write: esi -> src, eax -> x, ebx -> y, edx -> color
_write:
	; esi contains src of string
	mov edi, ebx
	imul edi, 160
	add edi, 0xB8000
	add edi, eax
	add edi, eax
	mov ah, dl
	.loop:		     ;цикл вывода сообщения
	lodsb			    ;считываем очередной символ строки
	test al, al		    ;если встретили 0
	jz   .exit		    ;прекращаем вывод
	stosw
	jmp  .loop
	.exit:
	ret

_hex_f:
	add edi, 7
	std
	;mbp
	mov ecx, 8
	.lp:
		mov edx, eax
		and eax, 0xF
		mov al, [.symbt+eax]
		stosb
		mov eax, edx
		shr eax, 4
	loop .lp
	cld
	ret
	.symbt: db '0123456789ABCDEF'

VMX_ECX = 100000b
PAGE_PRESENT = 01b
PAGE_WRITE = 10b
PAGE_SIZE = 10000000b

stri: db "00000000",0

GDT:
.Null:
	dq 0x0000000000000000             ; Null Descriptor - should be present.
.Code:
	dq 0x00209A0000000000             ; 64-bit code descriptor (exec/read).
	dq 0x0020920000000000             ; 64-bit data descriptor (read/write).
 
align 4
	dw 0                              ; Padding to make the "address of the GDT" field aligned on a 4-byte boundary
 
.Pointer:
	dw $ - GDT - 1                    ; 16-bit Size (Limit) of GDT.
	dd GDT                            ; 32-bit Base Address of GDT. (CPU will zero extend to 64-bit)

error_lm: db "This CPU doesn't support Long Mode (AMD64)",0
error_paging: db "This CPU doesn't support PAE paging", 0
error_vmx: db "This CPU doesn't support Intel VMX",0

include 'inc/consts.inc'

entry_pm:
	; >>> setting up all the basic stuff
	cli		     ; disabling interrupts
	; cs already defined
	mbp
	mov ax, sel_data
	mov ss, ax
	mov ds, ax
	mov es, ax
	mov esp, 0x7C00

	; >> checking cpuid
	; > vmx check
	cpuid_vmx:
		mov eax, 1
		cpuid
		test ecx, VMX_ECX
		jnz .exit_cpuid_vmx
			mov esi, error_vmx
			mov eax, 0
			mov ebx, 0
			mov edx, red
			call _write
		.exit_cpuid_vmx:
	; > lm support check
	cpuid_lm:
		mov eax, 0x80000000 ; extended cpuid?
		cpuid
		cmp eax, 0x80000001
		jc .nolm
		mov eax, 0x80000001
		cpuid
		test edx, 0x20000000 ; bit 29 - lm bit
		jnz .exit_cpuid_lm
		.nolm:
			mov esi, error_lm
			mov eax, 0
			mov ebx, 1
			mov edx, red
			call _write
			; that's essential
			jmp $
		.exit_cpuid_lm:

	; >> jumping to long mode
	; that's the thing:
	; at first - PTML4 (512x8 bytes = 0x1000)
	; each entry of PML4 covers 512 GiB of RAM...
	; one PDP will do :-)
	paging_clean: ; zero out 16KiB buffer
		mov edi, PAGING_PHYS_ADDR
		mov ecx, 0x1000
		xor eax, eax
		rep stosd

	paging_setup:
		mov edi, PAGING_PHYS_ADDR+PML4_OFF ; PML4T[i] -> PDP
		mov eax, PAGING_PHYS_ADDR+PDP_OFF
		or eax, PAGE_PRESENT
		stosd

		mov edi, PAGING_PHYS_ADDR+PDP_OFF ; PDP[i] -> PDT
		mov eax, PAGING_PHYS_ADDR+PD_OFF
		or eax, PAGE_PRESENT
		stosd

		mov edi, PAGING_PHYS_ADDR+PD_OFF ; PDT[i] -> PT
		mov eax, PAGE_SIZE or PAGE_PRESENT
		mov ecx, 512
		.lp:
			stosd
			add edi, 4
			add eax, 0x200000
		loop .lp

		; mov edi, PAGING_PHYS_ADDR+PD_OFF+0x200*8
		; mov eax, PAGE_SIZE or PAGE_PRESENT
		; stosd
		; mov edi, PAGING_PHYS_ADDR+0x3000
		; mov ecx, 512
		; mov eax, 0x3
		; .SetEntry:
		; 	stosd
		; 	add edi, 4
		; 	add eax, 0x1000
		; loop .SetEntry

	; demo:
	; 	mov edi, PAGING_PHYS_ADDR
	; 	mov ecx, 512
	; 	.lp:
	; 		mov eax, 0x101001
	; 		stosd
	; 		add edi, 4
	; 	loop .lp
	; 	mov edi, PAGING_PHYS_ADDR

cr4_pae_bit = 00100000b
lm_msr = 0xC0000080
efer_lme = 0x00000100
cr0_bit = 0x80000000

	lm_enable:
		mbp
		mov eax, cr4_pae_bit ; PAE 
		mov cr4, eax

		mov edx, PAGING_PHYS_ADDR+PML4_OFF
		mov cr3, edx

		mov ecx, lm_msr
		rdmsr
		or eax, efer_lme
		wrmsr

		mov ebx, cr0
		or ebx, cr0_bit
		mov cr0, ebx

		;jmp $

		lgdt [GDT.Pointer]
.exit:
	jmp sel_code32:LongMode
	; hey, we're in Long Mode
	; concatanate enterlm
	; TODO: fix it

; >>> селекторы дескрипторов (RPL=0, TI=0)
sel_zero    =   0000000b
sel_code32  =   0001000b
sel_data  	=   0010000b

; >>> colors
green = 0x0A
red = 0x04

; Here goes C flat binary?

; >>> boot sector signature
;finish:
;times 0x1FE-finish+start db 0
;db	 0x55, 0xAA ; ñèãíàòóðà çàãðóçî÷íîãî ñåêòîðà