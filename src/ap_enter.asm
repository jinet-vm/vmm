format binary
use16

include 'inc/consts.inc'

org AP_PHYS_LOAD

start:
	;cli                     ; Запрещаем прерывания
	mov ax, cs          ; Инициализируем сегментные регистры
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0x7C00      ; Т.к. стек растет в обратную сторону, то код не затрется
   
	mov ax, 0xB800
	mov gs, ax          ; Использовал для вывода текста прямой доступ к видеопамяти
   
	mov si, msg
	and si, 0xff
	xchg bx, bx
	.dest:
	mov cx, 100
	.lp:
	pusha
	call k_puts
	popa
	loop .lp               ; И уходим в бесконечный цикл
	jmp $
k_puts:
	

	xchg bx, bx
	lodsb
	test al, al
	jz .end_str
	mov ah, 0x0E
	mov bl, 0x                ; Серый на черном
	int 0x10
	jmp k_puts
.end_str:
	ret
 
msg     db 'AP loaded', 0
 
times 512-($-$$) db 0