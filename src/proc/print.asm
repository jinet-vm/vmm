public _print
; cprint(src,y,color) // null-terminated string
; print: src -> esi, y -> ebx, color -> edx
_print:
	; esi contains src
	; ebx contains y
	; edx contains color
	mov eax, 0
	call _write
	ret

include 'print.inc'

public print
print:
	push ebp
	mov ebp, esp
	Print [ebp+8], [ebp+12], [ebp+16]
	pop ebp
	ret