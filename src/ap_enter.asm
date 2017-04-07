format binary

include 'inc/consts.inc'

org AP_PHYS_LOAD

start:
	xchg bx, bx
	; jmp $
	cli		     ; disabling interrupts
	mov     ax, cs	  ; segment registers' init
	mov     ds, ax
	mov     es, ax
	mov     ss, ax
	mov     sp, 0x7C00      ; stack backwards => ok

	shl eax,4       ;умножаем на 16
	mov ebx,eax     ;копируем в регистр EBX
	; why?!

	; push dx, bx, ax, cx
	mov dx, 0 ; set cursor to top left-most corner of screen
	mov bh, 0 ; page
	mov ah, 0x2 ; ah = 2 => set cursor
	int 0x10 ; moving cursor
	mov cx, 2000 ; print 2000 = 80*45 chars
	mov bh, 0
	mov al, 'E'
	mov ah, 0x9
	int 0x10
	; pop cx, ax, bx, dx

	jmp $

times 512-($-$$) db 0