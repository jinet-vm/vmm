format ELF

include 'inc/macro.inc'
;include 'inc/procedures.inc'

; >>>> 16bit code

section '.text16' executable
Use16

org 0x7c00 ; why?! loop problems

public start
start:
	;xchg bx, bx
	mov [drive], dl
	cli		     ; disabling interrupts
	mov     ax, cs	  ; segment registers' init
	mov     ds, ax
	mov     es, ax
	mov     ss, ax
	mov     sp, 0x7C00      ; stack backwards => ok

	; shl eax,4       ;умножаем на 16
	; mov ebx,eax     ;копируем в регистр EBX
	; why?!

	; push dx, bx, ax, cx
	; mov dx, 0 ; set cursor to top left-most corner of screen
	; mov bh, 0 ; page
	; mov ah, 0x2 ; ah = 2 => set cursor
	; int 0x10 ; moving cursor
	; mov cx, 2000 ; print 2000 = 80*45 chars
	; mov bh, 0
	; mov ah, 0x9
	; int 0x10
	; pop cx, ax, bx, dx

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

	; mov si, vesa_sig
	; mov cx, 4
	; call print_str
	; jmp $

	mov si, DAP
	mov ah, 0x42
	mov dl, [drive] ; Floppy
	int 0x13

	; TODO: proper check
	; get vbe info -- mode
	mov ax, 0
	mov di, 0x7000
	mov cx, 256
	rep stosw ; zeroing
	mov si, vesa_sig
	mov cx, 4
	mov di, 0x7000
	rep movsb ; signature
	mov di, 0x7000
	mov ax, 0x4f00
	int 10h ; getting info
	cmp ax, 4fh
	jne $
	;xchg bx, bx
key_before:
	mov si, 0x700E ; address of modes pointer
	lodsd
	mov si, ax
	shr eax, 16
	mov ds, ax
	;xchg bx, bx
key_loop:
	; modes in ds:si
	lodsw
	test ax, 0xFFFF
	jz key_before
	mov cx, ax
	push cx
	;xchg bx, bx
	push esi
	call print_mode
	pop esi
	mov ah, 0h
	int 16h
	xchg bx, bx
	cmp ah, 0x48
	jne notup
.up: ; todo: zero up
	sub si, 4
	pop cx
	jmp key_loop
notup:
	cmp al, 0x0D
	jz setup
	pop cx
jmp key_loop
setup:
	xchg bx, bx
	mov ax, 4F02h
	pop bx
	or bx, 4000h
	int 10h ; set it
	xchg bx, bx
	; loading GDT
	lgdt    fword   [GDTR]

	; disable NMI
	in  al,70h
	or  al,80h
	out 70h,al

	; ; enable a20 no need for this
	; in  al,92h
	; or  al,2
	; out 92h,al
	
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
	.size:	db 0xff
	.zero:	db 0
	.num:	dw 100
	.addr:	dw 0x7e00
			dw 0
	.lba:	dd 1
			dd 0

; the same is done in desc.asm - for better migration to >1MB memspace

modes_ptr: dd 0

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

vesa_sig: db "vbe2"

; itoa(eax : number, ecx : width, ebx :  radix, edi : dest) -- stack
; Format number as string of width in radix. Returns pointer to string.
itoa:
	; mov eax, 420
	; push cx
	; push ax
	; push di
	; 	mov di, output
	; 	mov cx, 31
	; 	mov al, '0'
	; 	rep stosb
	; pop di
	; pop ax
	; pop cx
	;ret
	pushad
	mov ebp, esp
	; Start at end of output string and work backwards.
	; lea edi, [output + 32]
	add edi, ecx
	dec edi
	std
	; Load number and radix for division and iteration.
	; number eax
	; width ebx
	; radix ecx
	.loop:
		; Clear remainder / upper bits of dividend.
		xor edx, edx
		; Divide number by radix.
		div ebx
		; Use remainder to set digit in output string.
		xor esi, esi
		mov si, digits
		add si, dx
		push eax
		lodsb
		stosb
		pop eax
	loop .loop
	; The last movsb brought us too far back.
	lea eax, [edi + 1]
	cld
	mov esp, ebp
	popad
	ret
	digits: db "0123456789ABCDEF"


print_str: ; esi - ptr, ecx - count
	; push es
	; mov ax, 0xb800
	; mov es, ax
	; mov di, 0x3e8
	; mov ah, 0x0A
	; .lp:
	; 	lodsb
	; 	stosw
	; loop .lp
	; pop es
	; ret
	pushad
	mov bp, si
	mov dh, 10
	mov dl, 10
	mov bl, 0x0A
	mov al, 1
	mov ah, 13h
	int 10h
	popad
	ret

; print_mode(cx: mode)
print_mode:
	push cx
	movzx eax, cx
	mov ecx, 4
	mov ebx, 16
	mov edi, .str+5
	call itoa
	;xchg bx, bx
	mov ax, 0x4F01
	mov cx, [esp]
	mov di, 0x6F00
	int 10h
	;xchg bx, bx
	movzx eax, word [0x6F00+18] ; width
	mov ecx, 4
	mov ebx, 10
	mov edi, .str+12
	call itoa
	movzx eax, word [0x6F00+20] ; height 
	mov ecx, 4
	mov ebx, 10
	mov edi, .str+17
	call itoa
	movzx eax, byte [0x6F00+25] ; bpp
	mov ecx, 2
	mov ebx, 10
	mov edi, .str+22
	call itoa
	mov esi, .str
	mov ecx, 41
	;xchg bx, bx
	call print_str
	pop cx
	ret
	.str: db "mode 0000h: 0000x0000x00bpp (press ENTER)"

; >>>> 32bit code

section '.text32' executable align 100h
drive: db 0
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
	mov gs, ax
	mov fs, ax
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
		
		add edi, 4
		add eax, 0x1000
		stosd
		
		add edi, 4
		add eax, 0x1000
		stosd

		add edi, 4
		add eax, 0x1000
		stosd

		mov edi, PAGING_PHYS_ADDR+PD_OFF ; PDT[i] -> PT(size!)
		mov eax, PAGE_SIZE or PAGE_PRESENT
		mov ecx, 512*4 ; 4 gb
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
