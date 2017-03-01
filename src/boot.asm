format ELF

include 'inc/macro.inc'

; >>>> 16bit code

section '.text16' executable
Use16

org 0x7c00 ; why?! loop problems

public start
start:
	mbp
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

	mbp
	; loading entry_pm to RAM
	mov ah, 0x02    ; Read Disk Sectors
	mov al, 19    ; Read one sector only (512 bytes per sector)
	mov ch, 0x00    ; Track 0
	mov cl, 0x02    ; Sector 2
	mov dh, 0x00    ; Head 0
	mov dl, 0x00    ; Drive 0 (Floppy 1)
	mov bx, cs
	mov es, bx   ; Segment 0x2000
	mov bx, 0x7e00      ;  again remember segments but must be loaded from non immediate data
	int 13h
	mbp
	; mov si, DAP
	; mov ah, 42h
	; mov dl, 0x0 ; Floppy
	; int 0x13
	; mbp
	; jc
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

	mbp

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
	mbp
	; O32 jmp far
	db  66h ; O32
	db  0eah ; JMP FAR
	dd  0x7E00 ; offset
	dw  sel_code32 ; selector

DAP:
	.size:	db 10h
	.zero:	db 0
	.num:	dw 16
	.addr:	dw 0x7e00
			dw 0
	.lba:	dd 1
			dd 0

GDTTable:   ;таблица GDT
; zero seg
d_zero:		db  0,0,0,0,0,0,0,0     
; 32 bit code seg
d_code32:	db  0ffh,0ffh,0,0,0,10011010b,11001111b,0
; data
d_data:		db	0ffh, 0ffh, 0x00, 0, 0, 10010010b, 11001111b, 0x00
GDTSize     =   $-GDTTable
times 5 db 0,0,0,0,0,0,0,0 

GDTR:               ;загружаемое значение регистра GDTR
g_size:     dw  GDTSize-1   ;размер таблицы GDT
g_base:     dd  GDTTable           ;адрес таблицы GDT

; >>>> 32bit code

section '.text32' executable align 100h
; org     0x7E00
use32               ;32-битный код!!!

public entry_pm
extrn kernel_start

align   10h         ;код должен выравниваться по границе 16 байт
include 'inc/procedures.inc'
entry_pm:
	; >>> setting up all the basic stuff
	cli		     ; disabling interrupts
	; cs already defined
	mbp
	mov ax, sel_data
	mov ss, ax
	mov     esp, 0x7C00

	mov ax, sel_data
	mov ds, ax
	mov es, ax
	call kernel_start
	jmp $

; >>>> Descriptor stuff

public getGDTR
getGDTR:
	mov eax, GDTR
	ret

public setGDTR
setGDTR:
	lgdt fword [GDTR]
	ret

public setTR
setTR: ;(uint16_t segment)
	push ebp
	mov ebp, esp
	push eax
	mov eax, [ebp+8]
	ltr ax
	pop eax
	pop ebp
	ret

addr: times 20 db 0
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