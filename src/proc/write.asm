public _write
; write(src, x, y, color) // null-terminated string
; write: esi -> src, eax -> x, ebx -> y, edx -> color
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

include 'write.inc'

public write
write:
	push ebp
	mov ebp, esp
	Write [ebp+8],[ebp+12],[ebp+16],[ebp+24]
	pop ebp
	ret