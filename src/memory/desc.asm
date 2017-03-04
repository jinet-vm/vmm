format ELF

section '.data16' executable
use16

public GDTTable
GDTTable:   ;таблица GDT
; zero seg
d_zero:		db  0,0,0,0,0,0,0,0     
; 32 bit code seg
d_code32:	db  0ffh,0ffh,0,0,0,10011010b,11001111b,0
; data
d_data:		db	0ffh, 0ffh, 0x00, 0, 0, 10010010b, 11001111b, 0x00
GDTSize     =   $-GDTTable
times 5 db 0,0,0,0,0,0,0,0

public GDTR
GDTR:               ;загружаемое значение регистра GDTR
g_size:     dw  GDTSize-1   ;размер таблицы GDT
g_base:     dd  GDTTable           ;адрес таблицы GDT

section '.text32' executable
use32

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